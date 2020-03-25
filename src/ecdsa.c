#include "./../include/ecdsa.h"

/**
 * Check whether the given number is in the range [1..n-1].
 */
word inValidRange(const word *number) {
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
void ecdsaGenerateKeyPair(word *privateKey, word *pkx_mont, word *pky_mont) {
    word tmp[SIZE];

    /* Generate random scalar and point. */
    DHGenerateRandomSample(privateKey, pkx_mont, pky_mont, tmp);
    toCartesian(pkx_mont, pky_mont, tmp, pkx_mont, pky_mont);
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
    word nbBytesDigest = ((SIZE * sizeof(word) < 256 / 8) ? SIZE * sizeof(word) : 256 / 8);

    /* Compute digest of the message to sign. */
    sha3_HashBuffer(256, SHA3_FLAGS_NONE, message, nbBytes, e, nbBytesDigest);
    mod_add(e, zero, n, e);

    do {
        /* Generate random k. */
        DHGenerateRandomSample(k, X, Y, Z);

        /* Convert to cartesian coordinates. */
        toCartesian(X, Y, Z, r, y);
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
    word X[SIZE], Y[SIZE], Z[SIZE];
    word nbBytesDigest = ((SIZE * sizeof(word) < 256 / 8) ? SIZE * sizeof(word) : 256 / 8);

    if (!inValidRange(r) || !inValidRange(s))
        return 0;
    
    /* Compute digest of signed message. */
    sha3_HashBuffer(256, SHA3_FLAGS_NONE, message, nbBytes, e, nbBytesDigest);
    mod_add(e, zero, n, e);

    /* Compute inverse of s. */
    mod_inv(s, n, s_inv);

    /* Compute u1/u2. */
    montMul(e, s_inv, n, n_prime, u1);
    montMul(u1, rn_2, n, n_prime, u1);
    montMul(r, s_inv, n, n_prime, u2);
    montMul(u2, rn_2, n, n_prime, u2);

    /* Compute (x1, y1) and check whether it is correct. */
    shamirPointMultiply(u1, g_x_mont, g_y_mont, one_mont, u2, pkx_mont, pky_mont, one_mont, X, Y, Z);

    if (compareArrays(X, zero, SIZE) && compareArrays(Y, zero, SIZE))
        return 0;
    
    toCartesian(X, Y, Z, X, Y);
    mod_add(X, zero, n, X);

    return compareArrays(r, X, SIZE);
}