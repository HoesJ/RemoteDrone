#include "./params.h"

#ifndef EC_ARITHMETIC_H_
#define EC_ARITHMETIC_H_

/**
 * Checks whether the given point is on the elliptic curve. Returns 1 if
 * the given point is on the curve, 0 otherwise. The coordinates should be
 * given in the Montgomery domain.
 */
word isOnCurve(const word *x, const word *y);

/**
 * Convert the given X, Y and Z to cartesian coordinates. The parameter should be
 * given in the Montgomery domain. The result is computed in the normal domain.
 */
void toCartesian(const word *X, const word *Y, const word *Z, const word *p, const word *p_prime, word *x, word *y);

#endif