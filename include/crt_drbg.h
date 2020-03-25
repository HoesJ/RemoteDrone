#include <time.h>
#include "params.h"
#include "aes.h"
#include "mod_arithmetic.h"
#include "log_op.h"

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

/** 
 * Returns a specified amount of random bytes.
 */
void getRandomBytes(const word nbRandomBytes, void *randomBytes);

#endif