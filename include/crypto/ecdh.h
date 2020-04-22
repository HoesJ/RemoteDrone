#include "./../general/params.h"
#include "./../general/endian_convert.h"
#include "ec_arithmetic.h"
#include "crt_drbg.h"

#ifndef ECDH_H_
#define ECDH_H_

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
void ECDHSample(word *scalar, word *X, word *Y, word *Z);

/**
 * Generate a scalar and P = [scalar]G. The point P is expressed as cartesian coordinates in
 * the normal domain. The representation is fully little endian.
 */
void ECDHGenerateRandomSample(word *scalar, void *x, void *y);

/**
 * Multiply the given scalar and the given point. The point should be given in little endian
 * form and the result will be in this form as well. The data and result variables can be
 * the same.
 */
ECDHPointMultiply(const word *scalar, const void *x, const void *y, void *xRes, void *yRes);

#endif