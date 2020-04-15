#include "./../include/ecdh.h"

/**
 * Generate a scalar and P = [scalar]G. The point P is expressed as Jacobian coordinates in
 * the Montgomery domain.
 */
void ECDHSample(word *scalar, word *X, word *Y, word *Z) {
    /* Generate random number in [1..n-1]. */
    do {
        getRandomBytes(SIZE * sizeof(word), scalar);
    } while (!ECInValidRange(scalar));

    /* Calculate point multiplication. */
    pointMultiply(scalar, g_x_mont, g_y_mont, one_mont, X, Y, Z);
}

/**
 * Generate a scalar and P = [scalar]G. The point P is expressed as cartesian coordinates in
 * the normal domain. The representation is fully little endian.
 */
void ECDHGenerateRandomSample(word *scalar, void *x, void *y) {
    word X[SIZE], Y[SIZE], Z[SIZE];

    /* Generate sample and convert to little endian representation. */
    ECDHSample(scalar, X, Y, Z);
    toCartesian(X, Y, Z, X, Y);
    toLittleEndian(X, x, SIZE);
    toLittleEndian(Y, y, SIZE);
}

/**
 * Multiply the given scalar and the given point. The point should be given in little endian
 * form and the result will be in this form as well. The data and result variables can be
 * the same.
 */
ECDHPointMultiply(const word *scalar, const void *x, const void *y, void *xRes, void *yRes) {
    word X[SIZE], Y[SIZE], Z[SIZE], X_res[SIZE], Y_res[SIZE], Z_res[SIZE];

    /* Convert to word form in Jacobian coordinates. */
    toWordArray(x, X, SIZE);
    toWordArray(y, Y, SIZE);
    toJacobian(X, Y, X, Y, Z);

    /* Do point multiplication. */
    pointMultiply(scalar, X, Y, Z, X_res, Y_res, Z_res);
    toCartesian(X_res, Y_res, Z_res, X_res, Y_res);

    /* Convert result to little endian byte representation. */
    toLittleEndian(X_res, xRes, SIZE);
    toLittleEndian(Y_res, yRes, SIZE);
}