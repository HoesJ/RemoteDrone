#include <time.h>
#include <fcntl.h>
#include "./../general/params.h"
#include "aes.h"
#include "mod_arithmetic.h"
#include "logical_operations.h"

#ifndef CRT_DRBG_H_
#define CRT_DRBG_H_

/* First four constants are given in bytes. */
#define CTR_LEN             (128 / 8)
#define BLOCKLEN            (128 / 8)
#define KEYLEN              (128 / 8)
#define SEEDLEN             (256 / 8)
#define RESEED_INTERVAL      2e48
#define DF_MAX_NB_OUT_BYTES (512 / 8)
#define DF_MAX_NB_IN_BYTES  (1024 / 8)

/** 
 * Returns a specified amount of random bytes.
 */
void getRandomBytes(const size_t nbRandomBytes, void *randomBytes);

#endif