#include "./../include/aegis.h"

static const uint8_t constant[32] = { 0x0,0x1,0x01,0x02,0x03,0x05,0x08,0x0d,0x15,
									  0x22,0x37,0x59,0x90,0xe9,0x79,0x62,0xdb,0x3d,
									  0x18,0x55,0x6d,0xc2,0x2f,0xf1,0x20,0x11,0x31,
									  0x42,0x73,0xb5,0x28,0xdd };

/**
 * Creates the AEGIS context required for the encryption
 * or decryption. It initialises the key and an empty
 * state array. key is of size AES_BLOCK_NUMEL
 */
void createAegisContext(struct AEGIS_ctx* ctx, uint8_t* key) {
	ctx->key = key;
}

/**
 * Creates the AEGIS context required for the encryption
 * or decryption. It initialises the key, IV and an empty
 * state array. The IV can be reset later with "setIV"
 * key and iv are of size AES_BLOCK_NUMEL
 */
void createAegisContextIV(struct AEGIS_ctx* ctx, uint8_t* key, uint8_t* iv) {
	ctx->key = key;
	ctx->iv = iv;
}

/**
 * Sets the IV of the AEGIS encryption.
 * IV should have AES_BLOCK_NUMEL elements
 */
void setIV(struct AEGIS_ctx* ctx, uint8_t* iv) {
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
	word i;
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
void associatedData(struct AEGIS_ctx* ctx, uint8_t* ad, word adlen) {
	word i;

	for (i = 0; i < adlen; i++) 
		stateUpdate128(ctx->state, ad + i * 16);
}

/**
 * Performs the encryption of a single plaintext block of
 * 16 bytes long. It produces a single ciphertext block
 */
void xcrypt(struct AEGIS_ctx* ctx, uint8_t* input, word msglen, uint8_t* output, word encrypt) {
	word i;

	for (i = 0; i < msglen; i++) {
		AND128(output + i * 16, ctx->state + 32, ctx->state + 48);
		XOR128(output + i * 16, output + i * 16, ctx->state + 64);
		XOR128(output + i * 16, output + i * 16, ctx->state + 16);
		XOR128(output + i * 16, output + i * 16, input + i * 16);

		/* Update state */
		stateUpdate128(ctx->state, (encrypt == 1) ? input + i * 16 : output + i * 16);
	}
}

/**
 * Performs the finalization phase of the AEGIS encryption.
 * It takes the message length and ad length as input and
 * outputs a tag (of size AES_BLOCK_NUMEL.
 */
void finalization(struct AEGIS_ctx* ctx, word adlen, word msglen, uint8_t* tag) {
	uint8_t tmp[16];
	word i;

	((uint64_t*)tmp)[0] = adlen << 7;
	((uint64_t*)tmp)[1] = msglen << 7;

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
void aegisEncrypt(struct AEGIS_ctx* ctx, uint8_t* ad, word adlen, uint8_t* plain, word plainlen, uint8_t* cipher, uint8_t* tag) {
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
void aegisDecrypt(struct AEGIS_ctx* ctx, uint8_t* ad, word adlen, uint8_t* cipher, word cipherlen, uint8_t* plain, uint8_t* tag) {
	initialization(ctx);
	associatedData(ctx, ad, adlen);
	xcrypt(ctx, cipher, cipherlen, plain, 0);
	finalization(ctx, adlen, cipherlen, tag);
}