#include "./../../include/crypto/aegis.h"

static const uint8_t constant[32] = { 0x0,0x1,0x01,0x02,0x03,0x05,0x08,0x0d,0x15,
									  0x22,0x37,0x59,0x90,0xe9,0x79,0x62,0xdb,0x3d,
									  0x18,0x55,0x6d,0xc2,0x2f,0xf1,0x20,0x11,0x31,
									  0x42,0x73,0xb5,0x28,0xdd };

/**
 * Creates the AEGIS context required for the encryption
 * or decryption. It initialises the key and an empty
 * state array. key is of size AES_BLOCK_NUMEL
 */
void init_AEGIS_ctx(struct AEGIS_ctx* ctx, uint8_t* key) {
	ctx->key = key;
}

/**
 * Creates the AEGIS context required for the encryption
 * or decryption. It initialises the key, IV and an empty
 * state array. The IV can be reset later with "setIV"
 * key and iv are of size AES_BLOCK_NUMEL
 */
void init_AEGIS_ctx_IV(struct AEGIS_ctx* ctx, uint8_t* key, uint8_t* iv) {
	ctx->key = key;
	ctx->iv = iv;
}

/**
 * Performs an Aegis state update consisting of 5 AES rounds.
 * state is an array of size AEGIS_STATE_NUMEL
 * message is an array of size AES_BLOCK_NUMEL
 */
void stateUpdate128(uint8_t* state, uint8_t* message) {
	uint8_t temp[16];

	memcpy(temp, state + 64, 16);
	AESRound(state + 64, state + 48, state + 64);
	AESRound(state + 48, state + 32, state + 48);
	AESRound(state + 32, state + 16, state + 32);
	AESRound(state + 16, state + 0, state + 16);
	AESRound(state + 0, temp, state + 0);

	XOR128(state, state, message);
}

/**
 * Performs the AEGIS initialization phase.
 * Here the IV and the key are loaded into the state.
 * Note that the AEGIS context should first be created
 */
void initialization(struct AEGIS_ctx* ctx) {
	uint8_t i;
	uint8_t m[16];

	XOR128(ctx->state, ctx->key, ctx->iv);
	memcpy(ctx->state + 16, constant + 16, 16);
	memcpy(ctx->state + 32, constant, 16);
	XOR128(ctx->state + 48, ctx->key, constant);
	XOR128(ctx->state + 64, ctx->key, constant + 16);

	memcpy(m, ctx->key, 16);
	for (i = 0; i < 10; i++) {
		stateUpdate128(ctx->state, m);
		XOR128(m, m, ctx->iv);
	}
}

/**
 * Feeds the authenticated data to AEGIS state.
 * Each block has size AES_BLOCK_NUMEL
 */
void associatedData(struct AEGIS_ctx* ctx, uint8_t* ad, uint32_t adlen) {
	uint32_t i;
	uint8_t Zero[16] = { 0 };

	if (adlen == 0)
		return;

	for (i = 0; i + 16 < adlen; i += 16)
		stateUpdate128(ctx->state, ad + i);
	
	/* Handle last block differently to allow padding */
	memcpy(Zero, ad, adlen - i);
	stateUpdate128(ctx->state, (uint8_t*)Zero);
}

/**
 * Performs the encryption of a single plaintext block of
 * 16 bytes long. It produces a single ciphertext block
 */
void xcrypt(struct AEGIS_ctx* ctx, uint8_t* input, uint32_t msglen, uint8_t* output, uint8_t encrypt) {
	uint32_t i;
	uint8_t Zero[16] = { 0 };
	uint8_t outp[16] = { 0 };
	uint8_t inputBlock[16];

	for (i = 0; i + 16 < msglen; i += 16) {
		memcpy(inputBlock, input + i, 16);

		AND128(output + i, ctx->state + 32, ctx->state + 48);
		XOR128(output + i, output + i, ctx->state + 64);
		XOR128(output + i, output + i, ctx->state + 16);
		XOR128(output + i, output + i, inputBlock);

		/* Update state */
		stateUpdate128(ctx->state, (encrypt == 1) ? inputBlock : output + i);
	}

	/* Handle last block different to allow padding */
	memcpy(Zero, input + i, msglen - i);
	AND128(outp, ctx->state + 32, ctx->state + 48);
	XOR128(outp, outp, ctx->state + 64);
	XOR128(outp, outp, ctx->state + 16);
	XOR128(outp, outp, Zero);

	memcpy(output + i, outp, msglen - i);
	/* If last bits were zero, put to zero again - for decryption */
	memset(outp + msglen - i, 0, 16 - msglen + i);
	stateUpdate128(ctx->state, (encrypt == 1) ? Zero : outp);
}

/**
 * Performs the finalization phase of the AEGIS encryption.
 * It takes the message length and ad length as input and
 * outputs a tag (of size AES_BLOCK_NUMEL.
 */
void finalization(struct AEGIS_ctx* ctx, uint32_t adlen, uint32_t msglen, uint8_t* tag) {
	uint8_t tmp[16];
	uint8_t i;

	((uint64_t*)tmp)[0] = adlen << 3;
	((uint64_t*)tmp)[1] = msglen << 3;

	XOR128(tmp, tmp, ctx->state + 48);

	for (i = 0; i < 7; i++)
		stateUpdate128(ctx->state, tmp);

	XOR128(tag, ctx->state, ctx->state + 16);
	XOR128(tag, tag, ctx->state + 32);
	XOR128(tag, tag, ctx->state + 48);
	XOR128(tag, tag, ctx->state + 64);
}

/**
 * Performs the full AEGIS decryption on the initialized
 * context, given associated data and ciphertext
 * - ctx should have a key and fresh iv
 * - ad has adlen * AES_BLOCK_NUMEL elements
 * - plain has plainlen * AES_BLOCK_NUMEL elements
 * - cipher has plainlen * AES_BLOCK_NUMEL elements
 * - tag has 1 * AES_BLOCK_NUMEL elements
 */
void aegisEncrypt(struct AEGIS_ctx* ctx, uint8_t* ad, uint32_t adlen, uint8_t* plain, uint32_t plainlen, uint8_t* cipher, uint8_t* tag) {
	initialization(ctx);
	associatedData(ctx, ad, adlen);
	xcrypt(ctx, plain, plainlen, cipher, 1);
	finalization(ctx, adlen, plainlen, tag);
}

/**
 * Performs the full AEGIS decryption on the initialized
 * context, given associated data and ciphertext
 * - ctx should have a key and fresh iv
 * - ad has adlen * AES_BLOCK_NUMEL elements
 * - cipher has cipherlen * AES_BLOCK_NUMEL elements
 * - plain has plainlen * AES_BLOCK_NUMEL elements
 * - tag has 1 * AES_BLOCK_NUMEL elements
 */
void aegisDecrypt(struct AEGIS_ctx* ctx, uint8_t* ad, uint32_t adlen, uint8_t* cipher, uint32_t cipherlen, uint8_t* plain, uint8_t* tag) {
	initialization(ctx);
	associatedData(ctx, ad, adlen);
	xcrypt(ctx, cipher, cipherlen, plain, 0);
	finalization(ctx, adlen, cipherlen, tag);
}

/**
 * Encapsulates the AEGIS complexities to encrypt a message.
 * The message buffer should have the associated data at the beginning.
 * The plaintext to be encrypted should be after the associated data.
 * After both there should be room for a 16 byte MAC.
 * The ciphertext will replace the plaintext.
 *
 * adLen and plainLen are amount of bytes
 * adLen can be not a multiple of 16 and will be padded
 * plainLen should be a multiple of 16
 */
void aegisEncryptMessage(struct AEGIS_ctx* ctx, uint8_t* message, uint32_t adLen, uint32_t plainLen) {
	aegisEncrypt(ctx, message, adLen, message + adLen, plainLen, message + adLen, message + adLen + plainLen);
}

/**
 * Encapsulates the AEGIS complexities to decrypt a message.
 * The message buffer should have the associated data at the beginning.
 * The ciphertext to be decrypted should be after the associated data.
 * The plaintext will replace the ciphertext.
 *
 * The MAC at the end will be checked, and its corectness returned.
 *
 * adLen and plainLen are amount of bytes
 * adLen can be not a multiple of 16 and will be padded
 * plainLen should be a multiple of 16
 */
uint8_t aegisDecryptMessage(struct AEGIS_ctx* ctx, uint8_t* message, uint32_t adLen, uint32_t cipherLen) {
	uint8_t receivedMAC[16];

	aegisDecrypt(ctx, message, adLen, message + adLen, cipherLen, message + adLen, receivedMAC);

	return equalByteArrays(receivedMAC, message + adLen + cipherLen, 16);
}