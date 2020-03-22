#include "./../include/ecdsa.h"

/**
 * Check whether the given number is in the range [1..n-1].
 */
word inValidRange(word *number) {
    word i = 0;

    /* Not in valid range if number is zero. */
    if (compareArrays(number, zero, SIZE))
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
 * Generate a public/private key pair. The public key coordinates are represented
 * as numbers in the Montgomery domain.
 */
void generateKeyPair(word *privateKey, word *pkx_mont, word *pky_mont) {
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

/**
 * Sign the given message.
 */
void ecdsaSign(const uint8_t *message, const word nbBytes, const word *privateKey, word *r, word *s) {
    word e[SIZE];
    word k[SIZE], k_inv[SIZE];
    word X[SIZE], Y[SIZE], Z[SIZE], y[SIZE];

    /* Compute digest of the message to sign. */
    sha3_HashBuffer(256, SHA3_FLAGS_NONE, message, nbBytes, e, 256 / 8);
    mod_add(e, zero, n, e);

    do {
        /* Generate random k. */
        do {
            getRandomBytes(256 / 8, k);
        } while (!inValidRange(k));

        /* Compute new curve point. */
        pointMultiply(k, g_x_mont, g_y_mont, one_mont, p, p_prime, X, Y, Z);

        /* Convert to cartesian coordinates. */
        toCartesian(X, Y, Z, p, p_prime, r, y);
        mod_add(r, zero, n, r);
    } while (compareArrays(r, zero, SIZE));

    /* Compute s. */
    montMul(r, privateKey, n, n_prime, s);
    montMul(s, rn_2, n, n_prime, s);
    mod_add(s, e, n, s);
    mod_inv(k, n, k_inv);
    montMul(k_inv, s, n, n_prime, s);
    montMul(s, rn_2, n, n_prime, s);
}

/**
 * Check the signature on the the given message.
 */
word ecdsaCheck(const uint8_t *message, const word nbBytes, const word *pkx_mont, const word *pky_mont, const word *r, const word *s) {
    word e[SIZE];
    word s_inv[SIZE];
    word u1[SIZE], u2[SIZE];
    word X1[SIZE], Y1[SIZE], Z1[SIZE], X2[SIZE], Y2[SIZE], Z2[SIZE];

    if (!inValidRange(r) || !inValidRange(s))
        return 0;
    
    /* Compute digest of signed message. */
    sha3_HashBuffer(256, SHA3_FLAGS_NONE, message, nbBytes, e, 256 / 8);
    mod_add(e, zero, n, e);

    /* Compute inverse of s. */
    mod_inv(s, n, s_inv);

    /* Compute u1/u2. */
    montMul(e, s_inv, n, n_prime, u1);
    montMul(u1, rn_2, n, n_prime, u1);
    montMul(r, s_inv, n, n_prime, u2);
    montMul(u2, rn_2, n, n_prime, u2);

    /* Compute (x1, y1) and check whether it is correct. */
    shamirPointMultiply(u1, g_x_mont, g_y_mont, one_mont, u2, pkx_mont, pky_mont, one_mont, X1, Y1, Z1);

    if (compareArrays(Y1, zero, SIZE))
        return 0;
    
    toCartesian(X1, Y1, Z1, p, p_prime, X1, Y1);
    mod_add(X1, zero, n, X1);

    return compareArrays(r, X1, SIZE);
}