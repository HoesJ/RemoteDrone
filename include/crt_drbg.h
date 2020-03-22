#include "params.h"
#include "aes.h"
#include "mod_arithmetic.h"

#ifndef CRT_DRBG_H_
#define CRT_DRBG_H_

/* In bytes */
#define ctr_len         128 / 8
#define blocklen        128 / 8
#define keylen          128 / 8
#define seedlen         256 / 8        
#define reseed_interval 2e48

struct CTR_DRBG_ctx {
    uint8_t   key[blocklen];
    uint8_t   V[blocklen];
    word      reseed_ctr; 
};

/* Met block */
void CTR_DRBG_Update(const uint8_t *providedData, struct CTR_DRBG_ctx *state);

/* zonde rblock */
void CTR_DRBG_Instantiate(const uint8_t *entropyInput, const word entroptyInputLength, const uint8_t *nonce, const word nonceLength,
                          const uint8_t *personalizationString, const word personalizationStringLength, struct CTR_DRBG_ctx *state);

/* zonder block */
void CTR_DRBG_Reseed(const CTR_DRBG_ctx *workingState, const uint8_t *entropyInput);

/* met block */
void CTR_DRBG_Generate(struct CTR_DRBG_ctx *workingState, word requestedNbBits);

/* met block */ 
void Block_Cipher_df(const uint8_t *inputString, const word nbBitsToReturn, uint8_t *requestedBits);

#endif