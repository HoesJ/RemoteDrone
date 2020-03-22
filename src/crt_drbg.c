#include "./../include/crt_drbg.h"

static struct CTR_DRBG_ctx workingState = {
	.key = { 0 },
	.V = { 0 },
	.reseed_ctr = 1,
	.instantiated = 0
};

/**
 * Returns a seed of a defined length.
 */
void getSeed(uint8_t* buffer) {
	time_t rawtime;

	time(&rawtime);
	memcpy(buffer, &rawtime, sizeof(time_t));
}

/**
 * The derivation function used to process the input to the CTR_DRBG.
 */
void Block_Cipher_df(const uint8_t *inputString, const word inputLength, const word nbBytesToReturn, uint8_t *requestedBytes) {
	uint8_t S[BLOCKLEN + DF_MAX_NB_IN_BYTES + BLOCKLEN] = { 0 };
	uint8_t key[KEYLEN] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
	uint8_t temp[DF_MAX_NB_OUT_BYTES] = { 0 };
	struct AES_ctx aesCtx;
	word i;

	if (inputLength > DF_MAX_NB_IN_BYTES)
		return;
	if (nbBytesToReturn > DF_MAX_NB_OUT_BYTES)
		return;

	memcpy(S + BLOCKLEN, &inputLength, sizeof(word));                                /* S = L */
	memcpy(S + BLOCKLEN + 4, &nbBytesToReturn, sizeof(word) > 4 ? 4 : sizeof(word)); /* S = L || N */
	memcpy(S + BLOCKLEN + 8, inputString, inputLength);                              /* S = L || N || inputstring */
	S[BLOCKLEN + inputLength + 8] = 0x80;                                            /* S = L || N || inputstring || 0x80 */

	/* IV of zeros is already prepadded to S */
	AES_init_ctx(&aesCtx, key);

	/* BCC loop 1 */
	for (i = 0; i < inputLength + BLOCKLEN + 9; i += BLOCKLEN) {
		XOR128(temp, temp, S + i);
		AES_ECB_encrypt(&aesCtx, temp);
	}

	/* set IV to 1 */
	S[0] = 0x01;

	/* BCC loop 2 */
	for (i = 0; i < inputLength + BLOCKLEN + 9; i += BLOCKLEN) {
		XOR128(temp + BLOCKLEN, temp + BLOCKLEN, S + i);
		AES_ECB_encrypt(&aesCtx, temp + BLOCKLEN);
	}

	/* Take new key */
	AES_init_ctx(&aesCtx, temp);

	/* TAKE X by shifting in temp */
	memcpy(temp, temp + BLOCKLEN, BLOCKLEN);

	/* Final loop to create nbBytesToReturn */
	for (i = 0; i < nbBytesToReturn; i += BLOCKLEN) {
		if (i != 0)
			memcpy(temp + i, temp + i - BLOCKLEN, BLOCKLEN);    /* Copy so input to next iter can be shifted */
		AES_ECB_encrypt(&aesCtx, temp + i);
	}

	/* Copy to requestedBytes buffer */
	memcpy(requestedBytes, temp, nbBytesToReturn);
}

/**
 * Updates the RBG workingstate with the providedData
 * the provided data should have SEEDLEN.
  */
void CTR_DRBG_Update(const uint8_t *providedData) {
	uint8_t temp[SEEDLEN];
	struct AES_ctx aesCtx;

	add_overflow((word*)workingState.V, one, (word*)workingState.V);     /* V = V + 1 */
	memcpy(temp, workingState.V, BLOCKLEN);                              /* Store V in temp so temp contains AES output later */
	AES_init_ctx(&aesCtx, workingState.key);               				 /* Initialize AES key scheme */
	AES_ECB_encrypt(&aesCtx, temp);                         			 /* temp = temp || Block_Encrypt(V,key) */

	add_overflow((word*)workingState.V, one, (word*)workingState.V);     /* V = V + 1 */
	memcpy(temp + BLOCKLEN, workingState.V, BLOCKLEN);                   /* Store V in temp[16] so temp[16] contains AES output later */
	AES_ECB_encrypt(&aesCtx, temp + BLOCKLEN);                           /* temp = temp || Block_Encrypt(V,key) */

	XOR128(temp, temp, providedData);                       			 /* First part of temp = temp + provided data */
	XOR128(temp + BLOCKLEN, temp + BLOCKLEN,                			 /* Second 128 bits of temp = temp + provided data*/
		providedData + BLOCKLEN);

	memcpy(workingState.key, temp, KEYLEN);               				 /* Update key of state */
	memcpy(workingState.V, temp + KEYLEN, BLOCKLEN);      				 /* Update V of state */
}

/**
 * Instantiate the given working state with the given concatenation of entropy input, nonce and
 * personalization string.
 */
void CTR_DRBG_Instantiate(const uint8_t *entropyInputNoncePersonalizationString, const word inputLength) {
	uint8_t seed_material[SEEDLEN];
	word i;

	/* Compress seed material. */
	Block_Cipher_df(entropyInputNoncePersonalizationString, inputLength, SEEDLEN, seed_material);

	/* Instantiate key. */
	for (i = 0; i < KEYLEN; i++)
		workingState.key[i] = 0;

	/* Instantiate V. */
	for (i = 0; i < BLOCKLEN; i++)
		workingState.V[i] = 0;

	/* Compute new state. */
	CTR_DRBG_Update(seed_material);
	workingState.reseed_ctr = 1;
	workingState.instantiated = 1;
}

/**
 * Reseed the given working state with the given entropy input.
 */
void CTR_DRBG_Reseed(const uint8_t *entropyInput, const word inputLength) {
	uint8_t seed_material[SEEDLEN];

	/* Compute seed material. */
	Block_Cipher_df(entropyInput, inputLength, SEEDLEN, seed_material);

	/* Update state. */
	CTR_DRBG_Update(seed_material);
	workingState.reseed_ctr = 1;
}

/**
 * Generates a requested amount of random bytes.
 * Returns 1 if reseed is required.
 */
word CTR_DRBG_Generate(const word requestedNbBytes, uint8_t* randomBytes) {
	uint8_t additionalInput[SEEDLEN];
	uint8_t temp[BLOCKLEN];
	struct AES_ctx aesCtx;
	word i;

	if (workingState.reseed_ctr > RESEED_INTERVAL)
		return 1;

	AES_init_ctx(&aesCtx, workingState.key);

	/* Generate 16 bytes at a time */
	for (i = 0; i < requestedNbBytes; i += BLOCKLEN) {
		add_overflow((word*)workingState.V, one, (word*)workingState.V);            /* V = V + 1 */
		memcpy(temp, workingState.V, BLOCKLEN);                        				/* temp = V  */
		AES_ECB_encrypt(&aesCtx, temp);                                 			/* temp = AES(temp) */

		memcpy(randomBytes + i, temp, (requestedNbBytes - i) > BLOCKLEN ? BLOCKLEN : requestedNbBytes - i); /* Store generated output */
	}

	/* Update state */
	CTR_DRBG_Update(additionalInput);
	workingState.reseed_ctr += 1;
	return 0;
}

/**
 * Returns a specified amount of random bytes.
 */
void getRandomBytes(const word nbRandomBytes, uint8_t* randomBytes) {
	if (workingState.instantiated == 0) {
		uint8_t seed[82] = "This is a large personalization string that will partially be overwritten normally";
		getSeed(seed);
		CTR_DRBG_Instantiate(seed, 82);
	}

	if (CTR_DRBG_Generate(nbRandomBytes, randomBytes)) {
		uint8_t seed[82] = "This is a large personalization string that will partially be overwritten normally";
		getSeed(seed);
		CTR_DRBG_Reseed(seed, 82);
		CTR_DRBG_Generate(nbRandomBytes, randomBytes);
	}
}