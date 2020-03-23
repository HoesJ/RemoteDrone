#include "params.h"
#include "sha3.h"
#include "crt_drbg.h"
#include "compare_arrays.h"
#include "ec_arithmetic.h"

#ifndef ECDSA_H_
#define ECDSA_H_

/*****
 * Unless state otherwise in the documentation, all arguments should be represented
 * as either:
 * - (x, y): normal coordinates that are both in the normal domain.
 * - (X, Y, Z): Jacobian coordinates that are both in the Montgomery domain.
 *****/

/**
 * Generate a scalar and P = [scalar]G. The point P is expressed as Jacobian coordinates in
 * the Montgomery domain.
 */
void DHGenerateRandomSample(word *scalar, word *X, word *Y, word *Z);

/**
 * Generate a public/private key pair. The public key coordinates are represented
 * as numbers in the Montgomery domain.
 */
void ecdsaGenerateKeyPair(word *privateKey, word *pkx_mont, word *pky_mont);

/**
 * Sign the given message.
 */
void ecdsaSign(const uint8_t *message, const word nbBytes, const word *privateKey, word *r, word *s);

/**
 * Check the signature on the the given message.
 */
word ecdsaCheck(const uint8_t *message, const word nbBytes, const word *pkx_mont, const word *pky_mont, const word *r, const word *s);

#endif