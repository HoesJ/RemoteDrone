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
 * Returns the random bytes requested 
 */
void getRandomBytes(const word nbBytesToReturn, uint8_t* buffer);

/**
 * Returns a seed from an entropy sack
 */
void getSeed(uint8_t* buffer);

#endif