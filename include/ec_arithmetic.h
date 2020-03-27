#include "params.h"
#include "mod_arithmetic.h"
#include "compare_arrays.h"

#ifndef EC_ARITHMETIC_H_
#define EC_ARITHMETIC_H_

/*****
 * Unless state otherwise in the documentation, all arguments should be represented
 * as either:
 * - (x, y): normal coordinates that are both in the normal domain.
 * - (X, Y, Z): Jacobian coordinates that are both in the Montgomery domain.
 *****/

/**
 * Checks whether the given point is on the elliptic curve. Returns 1 if
 * the given point is on the curve, 0 otherwise. The coordinates should be
 * given in the normal domain.
 */
uint8_t isOnCurve(const word *x, const word *y);

/**
 * Convert the given x and y to Jacobian coordinates. The parameters should be
 * given in the normal domain. The result is computed in the Montgomery domain.
 */
void toJacobian(const word *x, const word *y, word *X, word *Y, word *Z);

/**
 * Convert the given X, Y and Z to cartesian coordinates. The parameters should be
 * given in the Montgomery domain. The result is computed in the normal domain.
 */
void toCartesian(const word *X, const word *Y, const word *Z, word *x, word *y);

/**
 * Compute the doubling of the given point (X, Y, Z). All coordinates are in the Montgomery domain.
 */
void pointDouble(const word *X, const word *Y, const word *Z, word *X_res, word *Y_res, word *Z_res);

/**
 * Compute the addition of the given points (X1, Y1, Z1) and (X2, Y2, Z2). All coordinates are in the Montgomery domain.
 */
void pointAdd(const word *X1, const word *Y1, const word *Z1, const word *X2, const word *Y2, const word *Z2, word *X_res,
              word *Y_res, word *Z_res);

/**
 * Compute the multiplication of the given scalar and the given point (X, Y, Z). All point coordinates are
 * in the Montgomery domain. The given result variables should not point to the same memory locations as
 * the given point (X, Y, Z).
 */
void pointMultiply(const word *scalar, const word *X, const word *Y, const word *Z, word *X_res, word *Y_res, word *Z_res);

/**
 * Compute k * (X1, Y1, Z1) + l * (X2, Y2, Z2).
 */
void shamirPointMultiply(const word *k, const word *X1, const word *Y1, const word *Z1, const word *l, const word *X2, const word *Y2,
                         const word *Z2, word *X_res, word *Y_res, word *Z_res);

#endif