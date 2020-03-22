#include "params.h"
#include "aes.h"
#include "mod_arithmetic.h"

#ifndef CRT_DRBG_H_
#define CRT_DRBG_H_

/* First four constants are given in bytes. */
#define CTR_LEN         128 / 8
#define BLOCKLEN        128 / 8
#define KEYLEN          128 / 8
#define SEEDLEN         256 / 8        
#define reseed_interval (2e48 < ALL_ONE_MASK ? 2e48 : ALL_ONE_MASK)
#define DF_MAX_NB_OUT_BYTES 512 / 8
#define DF_MAX_NB_IN_BYTES  1024 / 8

struct CTR_DRBG_ctx {
    uint8_t   key[KEYLEN];
    uint8_t   V[BLOCKLEN];
    word      reseed_ctr;
    word      instantiated;
};

/**
 * Updates the RBG workingstate with the providedData
 * the provided data should have SEEDLEN
  */
void CTR_DRBG_Update(const uint8_t *providedData);

/**
 * Instantiate the given working state with the given concatenation of entropy input, nonce and
 * personalization string.
 */
void CTR_DRBG_Instantiate(const uint8_t *entropyInputNoncePersonalizationString, const word inputLength);

/**
 * Reseed the given working state with the given entropy input.
 */
void CTR_DRBG_Reseed(const uint8_t *entropyInput, const word inputLength);

/**
 * Generates a requested amount of random bytes.
 */
word CTR_DRBG_Generate(const word requestedNbBytes, uint8_t* randomBytes);

/** 
 * The derivation function used to process the input to the CTR_DRBG
 */
void Block_Cipher_df(const uint8_t *inputString, const word inputLength, const word nbBytesToReturn, uint8_t *requestedBytes);

#endif