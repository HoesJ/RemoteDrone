#include "./../../include/crypto/ecdsa.h"

/**
 * Generate a public/private key pair. The public key coordinates are represented
 * as numbers in the Montgomery domain.
 */
void ecdsaGenerateKeyPair(word *privateKey, word *pkx_mont, word *pky_mont) {
    word tmp[SIZE];

    /* Generate random scalar and point. */
    ECDHSample(privateKey, pkx_mont, pky_mont, tmp);
    toCartesian(pkx_mont, pky_mont, tmp, pkx_mont, pky_mont);
    montMul(pkx_mont, rp_2, p, p_prime, pkx_mont);
    montMul(pky_mont, rp_2, p, p_prime, pky_mont);
}

/**
 * Sign the given message.
 */
void ecdsaSign(const void *message, const uint32_t nbBytes, const word *privateKey, void *r, void *s) {
    uint8_t eTmp[SIZE * sizeof(word)];
    word e[SIZE];
    word k[SIZE], k_inv[SIZE];
    word X[SIZE], Y[SIZE], Z[SIZE], y[SIZE];
    word rWord[SIZE], sWord[SIZE];
    uint8_t nbBytesDigest = ((SIZE * sizeof(word) < 256 / 8) ? SIZE * sizeof(word) : 256 / 8);

    /* Compute digest of the message to sign. */
    memset(eTmp, 0, SIZE * sizeof(word));
    sha3_HashBuffer(256, SHA3_FLAGS_NONE, message, nbBytes, eTmp, nbBytesDigest);
    toWordArray(eTmp, e, SIZE);
    mod_add(e, zero, n, e);

    do {
        /* Generate random k. */
        ECDHSample(k, X, Y, Z);

        /* Convert to cartesian coordinates. */
        toCartesian(X, Y, Z, rWord, y);
        mod_add(rWord, zero, n, rWord);
    } while (equalWordArrays(rWord, zero, SIZE));

    /* Compute s. */
    montMul(rWord, privateKey, n, n_prime, sWord);
    montMul(sWord, rn_2, n, n_prime, sWord);
    mod_add(sWord, e, n, sWord);
    mod_inv(k, n, k_inv);
    montMul(k_inv, sWord, n, n_prime, sWord);
    montMul(sWord, rn_2, n, n_prime, sWord);

    /* Convert to little endian form. */
    toLittleEndian(rWord, r, SIZE);
    toLittleEndian(sWord, s, SIZE);
}

/**
 * Check the signature on the given message.
 */
uint8_t ecdsaCheck(const void *message, const uint32_t nbBytes, const word *pkx_mont, const word *pky_mont, const void *r, const void *s) {
    uint8_t eTmp[SIZE * sizeof(word)];
    word e[SIZE];
    word s_inv[SIZE];
    word u1[SIZE], u2[SIZE];
    word X[SIZE], Y[SIZE], Z[SIZE];
    word rWord[SIZE], sWord[SIZE];
    uint8_t nbBytesDigest = ((SIZE * sizeof(word) < 256 / 8) ? SIZE * sizeof(word) : 256 / 8);

    /* Convert to word representation. */
    toWordArray(r, rWord, SIZE);
    toWordArray(s, sWord, SIZE);

    if (!ECInValidRange(rWord) || !ECInValidRange(sWord))
        return 0;
    
    /* Compute digest of the message to sign. */
    memset(eTmp, 0, SIZE * sizeof(word));
    sha3_HashBuffer(256, SHA3_FLAGS_NONE, message, nbBytes, eTmp, nbBytesDigest);
    toWordArray(eTmp, e, SIZE);
    mod_add(e, zero, n, e);

    /* Compute inverse of s. */
    mod_inv(sWord, n, s_inv);

    /* Compute u1/u2. */
    montMul(e, s_inv, n, n_prime, u1);
    montMul(u1, rn_2, n, n_prime, u1);
    montMul(rWord, s_inv, n, n_prime, u2);
    montMul(u2, rn_2, n, n_prime, u2);

    /* Compute (x1, y1) and check whether it is correct. */
    shamirPointMultiply(u1, g_x_mont, g_y_mont, one_mont, u2, pkx_mont, pky_mont, one_mont, X, Y, Z);

    if (equalWordArrays(X, zero, SIZE) && equalWordArrays(Y, zero, SIZE))
        return 0;
    
    toCartesian(X, Y, Z, X, Y);
    mod_add(X, zero, n, X);

    return equalWordArrays(rWord, X, SIZE);
}