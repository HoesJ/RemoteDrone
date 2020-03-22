#include "./../include/ecdsa.h"
#include "./../include/sha3.h"
#include "./../include/crt_drbg.h"
#include "./../include/compare_arrays.h"
#include "./../include/ec_arithmetic.h"

word isLessThan(word *a, word *b, word size) {
    word i = 0;

    for (i = 0; i < size; i++) {
        if (a[i] < b[i])
            return 1;
        else if (a[i] < b[i])
            return ;
    }

    return 0;
}

void ecdsaSign(const uint8_t *message, const word length, const word *privateKey, word *r, word *s) {
    word e[SIZE];
    word k[SIZE], k_inv[SIZE];
    word X[SIZE], Y[SIZE], Z[SIZE], y[SIZE];

    /* Compute digest of the message to sign. */
    sha3_HashBuffer(256, SHA3_FLAGS_NONE, message, length, e, 256 / 8);

    do {
        /* Generate random k. */
        do {
            CTR_DRBG_Generate(256 / 8, k);
        } while (!isLessThan(k, n, SIZE) || compareArrays(k, zero, SIZE));

        /* Compute new curve point. */
        pointMultiply(k, g_x_mont, g_y_mont, one_mont, p, p_prime, X, Y, Z);

        /* Convert to cartesian coordinates. */
        toCartesian(X, Y, Z, p, p_prime, r, y);
    } while (!compareArrays(r, zero, SIZE));

    
    mod_inv(k, n, k_inv);
}