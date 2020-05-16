#include "./../../include/crypto/crt_drbg.h"

/* Working state of random number generator. */
struct CTR_DRBG_ctx {
    uint8_t  key[KEYLEN];
    uint8_t  V[BLOCKLEN];
    uint64_t reseed_ctr;
};

static struct CTR_DRBG_ctx workingState;
static uint8_t 			   instantiated = 0;

/* Variables for entropy source. */
static uint8_t entropy_buffer[256];
static ssize_t buffer_size = sizeof(entropy_buffer);
static size_t  ind = sizeof(entropy_buffer);

/**
 * Check whether the entropy source still has entropy left.
 */
uint8_t hasEntropyLeft() {
	return ind < buffer_size;
}

/**
 * Generates a seed of maximally the given length. Returns the length in bytes of
 * the seed generated.
 */
size_t getSeed(uint8_t *buffer, size_t length) {
	size_t nbBytesLeft;
	size_t generatedLength;
	int randomData;

	/* Generate seed based on the UNIX input device or Windows current time. */
	if (!hasEntropyLeft()) {
		ind = 0;
		randomData = open("/dev/urandom", O_RDONLY);
		if (randomData < 0)
			return 0;
		else {
			buffer_size = read(randomData, entropy_buffer, sizeof(entropy_buffer));
			if (buffer_size <= 0) {
				buffer_size = 0;
				return 0;
			}
		}
	}

	/* Copy seed to buffer. */
	nbBytesLeft = buffer_size - ind;
	if (nbBytesLeft < length)
		generatedLength = nbBytesLeft;
	else
		generatedLength = length;
	
	memcpy(buffer, entropy_buffer + ind, generatedLength);
	ind += generatedLength;
	return generatedLength;
}

/**
 * The derivation function used to process the input to the CTR_DRBG.
 */
void Block_Cipher_df(const uint8_t *inputString, const size_t inputLength, const size_t nbBytesToReturn, uint8_t *requestedBytes) {
	uint8_t S[BLOCKLEN + DF_MAX_NB_IN_BYTES + BLOCKLEN] = { 0 };
	uint8_t key[KEYLEN] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
	uint8_t temp[DF_MAX_NB_OUT_BYTES] = { 0 };
	struct AES_ctx aesCtx;
	size_t i;

	if (inputLength > DF_MAX_NB_IN_BYTES)
		return;
	if (nbBytesToReturn > DF_MAX_NB_OUT_BYTES)
		return;

	memcpy(S + BLOCKLEN, &inputLength, sizeof(size_t));                                	 /* S = L */
	memcpy(S + BLOCKLEN + 4, &nbBytesToReturn, sizeof(size_t) > 4 ? 4 : sizeof(size_t)); /* S = L || N */
	memcpy(S + BLOCKLEN + 8, inputString, inputLength);                                  /* S = L || N || inputstring */
	S[BLOCKLEN + inputLength + 8] = 0x80;                                                /* S = L || N || inputstring || 0x80 */

	/* IV of zeros is already prepadded to S */
	AES_init_ctx(&aesCtx, key);

	/* BCC loop 1 */
	for (i = 0; i < inputLength + BLOCKLEN + 9; i += BLOCKLEN) {
		XOR128(temp, temp, S + i);
		AES_ECB_encrypt(&aesCtx, temp);
	}

	/* Set IV to 1 */
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
			memcpy(temp + i, temp + i - BLOCKLEN, BLOCKLEN);    					  		/* Copy so input to next iter can be shifted */
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

	add_overflow((word*)workingState.V, one, (word*)workingState.V, BLOCKLEN / sizeof(word)); /* V = V + 1 */
	memcpy(temp, workingState.V, BLOCKLEN);                              					  /* Store V in temp so temp contains AES output later */
	AES_init_ctx(&aesCtx, workingState.key);               				 					  /* Initialize AES key scheme */
	AES_ECB_encrypt(&aesCtx, temp);                         			 					  /* temp = temp || Block_Encrypt(V,key) */

	add_overflow((word*)workingState.V, one, (word*)workingState.V, BLOCKLEN / sizeof(word)); /* V = V + 1 */
	memcpy(temp + BLOCKLEN, workingState.V, BLOCKLEN);                   					  /* Store V in temp[16] so temp[16] contains AES output later */
	AES_ECB_encrypt(&aesCtx, temp + BLOCKLEN);                           					  /* temp = temp || Block_Encrypt(V,key) */

	XOR128(temp, temp, providedData);                       			 					  /* First part of temp = temp + provided data */
	XOR128(temp + BLOCKLEN, temp + BLOCKLEN,                			 					  /* Second 128 bits of temp = temp + provided data*/
		providedData + BLOCKLEN);

	memcpy(workingState.key, temp, KEYLEN);               				 					  /* Update key of state */
	memcpy(workingState.V, temp + KEYLEN, BLOCKLEN);      				 					  /* Update V of state */
}

/**
 * Instantiate the given working state with the given concatenation of entropy input, nonce and
 * personalization string.
 */
void CTR_DRBG_Instantiate(const uint8_t *entropyInputNoncePersonalizationString, const size_t inputLength) {
	uint8_t seed_material[SEEDLEN];
	size_t i;

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
	instantiated = 1;
}

/**
 * Reseed the given working state with the given entropy input.
 */
void CTR_DRBG_Reseed(const uint8_t *entropyInput, const size_t inputLength) {
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
uint8_t CTR_DRBG_Generate(const size_t requestedNbBytes, void *randomBytes) {
	uint8_t additionalInput[SEEDLEN];
	uint8_t temp[BLOCKLEN];
	struct AES_ctx aesCtx;
	size_t i;

	if (workingState.reseed_ctr > RESEED_INTERVAL)
		return 1;

	AES_init_ctx(&aesCtx, workingState.key);

	/* Generate 16 bytes at a time */
	for (i = 0; i < requestedNbBytes; i += BLOCKLEN) {
		add_overflow((word*)workingState.V, one, (word*)workingState.V, BLOCKLEN / sizeof(word));           			/* V = V + 1 */
		memcpy(temp, workingState.V, BLOCKLEN);                        													/* temp = V  */
		AES_ECB_encrypt(&aesCtx, temp);                                 												/* temp = AES(temp) */

		memcpy(((uint8_t*)randomBytes) + i, temp, (requestedNbBytes - i) > BLOCKLEN ? BLOCKLEN : requestedNbBytes - i); /* Store generated output */
	}

	/* Update state */
	CTR_DRBG_Update(additionalInput);
	workingState.reseed_ctr += 1;
	return 0;
}

/**
 * Returns a specified amount of random bytes.
 */
void getRandomBytes(const size_t nbRandomBytes, void *randomBytes) {
	uint8_t seed[82] = "This is a large personalization string that will partially be overwritten normally";
	size_t seedLength;

	/* Instantiate random number generator. */
	if (instantiated == 0) {
		getSeed(seed, 82);
		CTR_DRBG_Instantiate(seed, 82);
		while (hasEntropyLeft()) {
			seedLength = getSeed(seed, 82);
			CTR_DRBG_Reseed(seed, seedLength);
		}
	}

	/* Reseed if necessary. */
	if (CTR_DRBG_Generate(nbRandomBytes, randomBytes)) {
		do {
			seedLength = getSeed(seed, 82);
			CTR_DRBG_Reseed(seed, seedLength);
		} while (hasEntropyLeft());

		CTR_DRBG_Generate(nbRandomBytes, randomBytes);
	}
}