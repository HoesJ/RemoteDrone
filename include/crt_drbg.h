#include <time.h>
#include "params.h"
#include "aes.h"
#include "mod_arithmetic.h"

#ifndef CRT_DRBG_H_
#define CRT_DRBG_H_

/* First four constants are given in bytes. */
#define CTR_LEN             128 / 8
#define BLOCKLEN            128 / 8
#define KEYLEN              128 / 8
#define SEEDLEN             256 / 8        
#define RESEED_INTERVAL     (2e48 < ALL_ONE_MASK ? 2e48 : ALL_ONE_MASK)
#define DF_MAX_NB_OUT_BYTES 512 / 8
#define DF_MAX_NB_IN_BYTES  1024 / 8

struct CTR_DRBG_ctx {
    uint8_t   key[KEYLEN];
    uint8_t   V[BLOCKLEN];
    word      reseed_ctr;
    word      instantiated;
};

/** 
 * Returns a specified amount of random bytes.
 */
void getRandomBytes(const word nbRandomBytes, uint8_t *randomBytes);

/** 
 * Returns a seed of a defined length.
 */
void getSeed(uint8_t *buffer);

#endif