#include "params.h"
#include "ec_arithmetic.h"
#include "crt_drbg.h"

#ifndef DH_H_
#define DH_H_

/*****
 * Unless state otherwise in the documentation, all arguments should be represented
 * as either:
 * - (x, y): normal coordinates that are both in the normal domain.
 * - (X, Y, Z): Jacobian coordinates that are both in the Montgomery domain.
 *****/

/**
 * Check whether the given number is in the range [1..n-1].
 */
word ECInValidRange(const word *number);

/**
 * Generate a scalar and P = [scalar]G. The point P is expressed as Jacobian coordinates in
 * the Montgomery domain.
 */
void ECDHGenerateRandomSample(word *scalar, word *X, word *Y, word *Z);

#endif