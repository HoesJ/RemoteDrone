#include "./../include/ecdsa.h"
#include "./../include/sha3.h"
#include "./../include/crt_drbg.h"
#include "./../include/compare_arrays.h"
#include "./../include/ec_arithmetic.h"

word inValidRange(word *number) {
    word i = 0;

    for (i = 0; i < SIZE; i++) {
        if (number[i] < n[i])
            return 1;
        else if (number[i] > b[i])
            return 0;
    }

    return (!compareArrays(number, zero, SIZE));
}

void generateKeyPair(word *privateKey, word *pkx_mont, word pky_mont) {
    word tmp[SIZE];

    /* Generate private key. */
    do {
        getRandomBytes(256 / 8, privateKey);
    } while (!inValidRange(privateKey));

    /* Calculate public key. */
    pointMultiply(privateKey, g_x_mont, g_y_mont, one_mont, p, p_prime, pkx_mont, pky_mont, tmp);
    toCartesian(pkx_mont, pky_mont, tmp, p, p_prime, pkx_mont, pky_mont);
    montMul(pkx_mont, rp_2, p, p_prime, pkx_mont);
    montMul(pky_mont, rp_2, p, p_prime, pky_mont);
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
            getRandomBytes(256 / 8, k);
        } while (!inValidRange(k));

        /* Compute new curve point. */
        pointMultiply(k, g_x_mont, g_y_mont, one_mont, p, p_prime, X, Y, Z);

        /* Convert to cartesian coordinates. */
        toCartesian(X, Y, Z, p, p_prime, r, y);
    } while (compareArrays(r, zero, SIZE));

    /* Compute s. */
    montMul(r, rn_2, n, n_prime, s);
    montMul(s, privateKey, n, n_prime, s);
    mod_add(e, s, n, s);
    mod_inv(k, n, k_inv);
    montMul(k_inv, s, n, n_prime, s);
    montMul(s, rn_2, n, n_prime, s);
}

word ecdsaCheck(const uint8_t *message, const word length, const word *pkx_mont, const word *pky_mont, const word *r, const word *s) {
    word e[SIZE];
    word u1[SIZE], u2[SIZE];
    word s_inv[SIZE];
    word X1[SIZE], Y1[SIZE], Z1[SIZE], X2[SIZE], Y2[SIZE], Z2[SIZE];

    if (!inValidRange(r) || !inValidRange(s))
        return 0;
    
    /* Compute digest of signed message. */
    sha3_HashBuffer(256, SHA3_FLAGS_NONE, message, length, e, 256 / 8);

    /* Compute inverse of s. */
    mod_inv(s, n, s_inv);

    /* Compute u1/u2. */
    montMul(e, s_inv, n, n_prime, u1);
    montMul(u1, rn_2, n, n_prime, u1);
    montMul(r, s_inv, n, n_prime, u2);
    montMul(u2, rn_2, n, n_prime, u2);

    /* Compute point that should be equal to the point at infinity. */
    pointMultiply(u1, g_x_mont, g_y_mont, one_mont, p, p_prime, X1, Y1, Z1);
    pointMultiply(u2, pkx_mont, pky_mont, one_mont, p, p_prime, X2, Y2, Z2);
    pointAdd(X1, Y1, Z1, X2, Y2, Z2, p, p_prime, X1, Y1, Z1);

    return compareArrays(Y1, zero, SIZE);
}