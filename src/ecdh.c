#include "./../include/ecdh.h"

/**
 * Check whether the given number is in the range [1..n-1].
 */
word ECInValidRange(const word *number) {
    word i = 0;

    /* Not in valid range if number is zero. */
    if (compareWordArrays(number, zero, SIZE))
        return 0;

    /* Check if number is smaller than n. */
    for (i = 0; i < SIZE; i++) {
        if (number[i] < n[i])
            return 1;
        else if (number[i] > b[i])
            return 0;
    }

    return 0;
}

/**
 * Generate a scalar and P = [scalar]G. The point P is expressed as Jacobian coordinates in
 * the Montgomery domain.
 */
void ECDHGenerateRandomSample(word *scalar, word *X, word *Y, word *Z) {
    /* Generate random number in [1..n-1]. */
    do {
        getRandomBytes(SIZE * sizeof(word), scalar);
    } while (!ECInValidRange(scalar));

    /* Calculate point multiplication. */
    pointMultiply(scalar, g_x_mont, g_y_mont, one_mont, X, Y, Z);
}