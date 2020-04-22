#include "./../general/params.h"
#include "./../general/compare_arrays.h"
#include "./../general/endian_convert.h"
#include "sha3.h"
#include "crt_drbg.h"
#include "ec_arithmetic.h"
#include "ecdh.h"

#ifndef ECDSA_H_
#define ECDSA_H_

/*****
 * Unless state otherwise in the documentation, all arguments should be represented
 * as either:
 * - (x, y): normal coordinates that are both in the normal domain.
 * - (X, Y, Z): Jacobian coordinates that are both in the Montgomery domain.
 *****/

/**
 * Generate a public/private key pair. The public key coordinates are represented
 * as numbers in the Montgomery domain.
 */
void ecdsaGenerateKeyPair(word *privateKey, word *pkx_mont, word *pky_mont);

/**
 * Sign the given message.
 */
void ecdsaSign(const void *message, const word nbBytes, const word *privateKey, void *r, void *s);

/**
 * Check the signature on the given message.
 */
uint8_t ecdsaCheck(const void *message, const word nbBytes, const word *pkx_mont, const word *pky_mont, const void *r, const void *s);

#endif