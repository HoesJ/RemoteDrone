#include "./../include/ecdh.h"

/**
 * Generate a scalar and P = [scalar]G. The point P is expressed as Jacobian coordinates in
 * the Montgomery domain.
 */
void ECDHGenerateRandomSample(word *scalar, word *X, word *Y, word *Z) {
    /* Generate random number in [1..n-1]. */
    do {
        getRandomBytes(SIZE * sizeof(word), scalar);
    } while (!inValidRange(scalar));

    /* Calculate point multiplication. */
    pointMultiply(scalar, g_x_mont, g_y_mont, one_mont, X, Y, Z);
}