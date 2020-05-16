#include "./../include/test/test.h"
#include "./../include/general/print_number.h"

void test_mont_parameters(uint32_t *nbTest) {
    word one_mont[SIZE], res[SIZE];
    
    /* Test p. */
    montMul(one, rp_2, p, p_prime, one_mont);
    montMul(one_mont, one_mont, p, p_prime, res);
    montMul(res, one, p, p_prime, res);
    
    assert(equalWordArrays(res, one, SIZE));
    printf("Test %u - Montgomery parameters passed.\n", (*nbTest)++);

    /* Test n. */
    montMul(one, rn_2, n, n_prime, one_mont);
    montMul(one_mont, one_mont, n, n_prime, res);
    montMul(res, one, n, n_prime, res);
    
    assert(equalWordArrays(res, one, SIZE));
    printf("Test %u - Montgomery parameters passed.\n", (*nbTest)++);
}

void test_ec_curve(uint32_t *nbTest) {
    /* Curve should satisfy b * b * c = -27 (mod p). */

    word b_mont[SIZE], c_mont[SIZE], res[SIZE];
    word min_27[SIZE];
    uint8_t min_27Bytes[SIZE * sizeof(word)] = { 0xE4, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
                                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF };
    toWordArray(min_27Bytes, min_27, SIZE);
    
    montMul(b, rp_2, p, p_prime, b_mont);
    montMul(c, rp_2, p, p_prime, c_mont);

    montMul(b_mont, b_mont, p, p_prime, res);
    montMul(res, c_mont, p, p_prime, res);
    montMul(res, one, p, p_prime, res);
    
    assert(equalWordArrays(res, min_27, SIZE));
    printf("Test %u - Curve parameters passed.\n", (*nbTest)++);
}

void test_generator(uint32_t *nbTest) {
    assert(isOnCurve(g_x, g_y));
    printf("Test %u - Generator on curve passed.\n", (*nbTest)++);
}

void test_cartesian(uint32_t *nbTest) {
    word x_res[SIZE], y_res[SIZE];

    toCartesian(g_x_mont, g_y_mont, one_mont, x_res, y_res);

    assert(equalWordArrays(x_res, g_x, SIZE));
    assert(equalWordArrays(y_res, g_y, SIZE));
    printf("Test %u - Conversion to cartesian coordinates passed.\n", (*nbTest)++);
}

void test_jacobian_cartesian(uint32_t *nbTest) {
    word X[SIZE], Y[SIZE], Z[SIZE], x[SIZE], y[SIZE];

    toJacobian(g_x, g_y, X, Y, Z);
    toCartesian(X, Y, Z, x, y);

    assert(equalWordArrays(x, g_x, SIZE));
    assert(equalWordArrays(y, g_y, SIZE));
    printf("Test %u - Conversion to Jacobian/cartesian coordinates passed.\n", (*nbTest)++);
}

void test_montMul_zero(uint32_t *nbTest) {
    word res[SIZE];

    montMul(zero, zero, p, p_prime, res);

    assert(equalWordArrays(res, zero, SIZE));
    printf("Test %u - Montgomery multiplication with zero coordinates passed.\n", (*nbTest)++);
}

void test_pointDouble(uint32_t *nbTest) {
    word X_res[SIZE], Y_res[SIZE], Z_res[SIZE], x_res[SIZE], y_res[SIZE];

    pointDouble(g_x_mont, g_y_mont, one_mont, X_res, Y_res, Z_res);
    toCartesian(X_res, Y_res, Z_res, x_res, y_res);

    assert(isOnCurve(x_res, y_res));
    printf("Test %u - Point doubling on curve passed.\n", (*nbTest)++);
}

void test_pointAdd(uint32_t *nbTest) {
    word X_res[SIZE], Y_res[SIZE], Z_res[SIZE], x_res[SIZE], y_res[SIZE];

    pointDouble(g_x_mont, g_y_mont, one_mont, X_res, Y_res, Z_res);
    pointAdd(g_x_mont, g_y_mont, one_mont, X_res, Y_res, Z_res, X_res, Y_res, Z_res);
    toCartesian(X_res, Y_res, Z_res, x_res, y_res);

    assert(isOnCurve(x_res, y_res));
    printf("Test %u - Point addition on curve passed.\n", (*nbTest)++);
}

void test_pointDoubleAddConsistency(uint32_t *nbTest) {
    word X_res1[SIZE], Y_res1[SIZE], Z_res1[SIZE], x_res1[SIZE], y_res1[SIZE];
    word X_res2[SIZE], Y_res2[SIZE], Z_res2[SIZE], x_res2[SIZE], y_res2[SIZE];

    pointDouble(g_x_mont, g_y_mont, one_mont, X_res1, Y_res1, Z_res1);
    toCartesian(X_res1, Y_res1, Z_res1, x_res1, y_res1);

    pointAdd(g_x_mont, g_y_mont, one_mont, g_x_mont, g_y_mont, one_mont, X_res2, Y_res2, Z_res2);
    toCartesian(X_res2, Y_res2, Z_res2, x_res2, y_res2);

    assert(equalWordArrays(x_res1, x_res2, SIZE));
    assert(equalWordArrays(y_res1, y_res2, SIZE));
    printf("Test %u - Point doubling/addition consistency passed.\n", (*nbTest)++);
}

void test_multiplyComm(uint32_t *nbTest) {
    word scalar1[SIZE];
    uint8_t scalar1Bytes[SIZE * sizeof(word)] = { 0x54, 0x87, 0x57, 0x04, 0xFE, 0x4D, 0xAB, 0x00, 0x54, 0x87, 0x57, 0x04, 0x00, 0x00, 0x00, 0x00,
                                                  0xFC, 0xDE, 0x0F, 0x01, 0xFE, 0x4D, 0xAB, 0x00, 0x54, 0x87, 0x57, 0x04, 0xF4, 0x4D, 0xAB, 0x00 };
    toWordArray(scalar1Bytes, scalar1, SIZE);

    word scalar2[SIZE];
    uint8_t scalar2Bytes[SIZE * sizeof(word)] = { 0x10, 0x47, 0x57, 0x04, 0xFE, 0x4D, 0xAB, 0x00, 0x54, 0x77, 0x58, 0x04, 0x00, 0xE0, 0xFF, 0x0F,
                                                  0xFC, 0xDE, 0x0F, 0x01, 0xFE, 0x4D, 0xAB, 0x00, 0x54, 0x87, 0x57, 0x04, 0xF4, 0x4D, 0xAB, 0x00 };
    toWordArray(scalar2Bytes, scalar2, SIZE);
    
    word X_res_tmp[SIZE], Y_res_tmp[SIZE], Z_res_tmp[SIZE],
         X_res1[SIZE],    Y_res1[SIZE],    Z_res1[SIZE],
         X_res2[SIZE],    Y_res2[SIZE],    Z_res2[SIZE],
         x_res1[SIZE],    y_res1[SIZE],
         x_res2[SIZE],    y_res2[SIZE];

    pointMultiply(scalar1, g_x_mont, g_y_mont, one_mont, X_res_tmp, Y_res_tmp, Z_res_tmp);
    pointMultiply(scalar2, X_res_tmp, Y_res_tmp, Z_res_tmp, X_res1, Y_res1, Z_res1);

    pointMultiply(scalar2, g_x_mont, g_y_mont, one_mont, X_res_tmp, Y_res_tmp, Z_res_tmp);
    pointMultiply(scalar1, X_res_tmp, Y_res_tmp, Z_res_tmp, X_res2, Y_res2, Z_res2);

    toCartesian(X_res1, Y_res1, Z_res1, x_res1, y_res1);
    toCartesian(X_res2, Y_res2, Z_res2, x_res2, y_res2);
    
    assert(equalWordArrays(x_res1, x_res2, SIZE));
    assert(equalWordArrays(y_res1, y_res2, SIZE));

    assert(isOnCurve(x_res1, y_res1));
    printf("Test %u - Commutative point multiplication passed.\n", (*nbTest)++);
}

void test_curveOrder(uint32_t *nbTest) {
    word X[SIZE], Y[SIZE], Z[SIZE];

    pointMultiply(n, g_x_mont, g_y_mont, one_mont, X, Y, Z);

    assert(equalWordArrays(X, zero, SIZE));
    assert(equalWordArrays(Y, zero, SIZE));

    printf("Test %u - High order subgroup passed.\n", (*nbTest)++);
}

void test_online(uint32_t *nbTest) {
    word k[SIZE];
    uint8_t kBytes[SIZE * sizeof(word)] = { 0x13, 0x0E, 0x59, 0x43, 0xCA, 0xCD, 0x46, 0x72, 0x74, 0xDD, 0x4C, 0x3D, 0x89, 0x9D, 0x15, 0x00,
                                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    toWordArray(kBytes, k, SIZE);

    word x[SIZE];
    uint8_t xBytes[SIZE * sizeof(word)] = { 0x64, 0x32, 0x80, 0xF2, 0x32, 0xCD, 0x98, 0xB4, 0x12, 0xAB, 0xAD, 0xB5, 0x41, 0x32, 0xCC, 0x65,
                                            0xF6, 0x29, 0x67, 0x3F, 0x00, 0xA5, 0x7F, 0x6D, 0x5E, 0xC2, 0x6C, 0x07, 0x6A, 0x04, 0x7E, 0x1B };
    toWordArray(xBytes, x, SIZE);

    word y[SIZE];
    uint8_t yBytes[SIZE * sizeof(word)] = { 0x7B, 0x45, 0x7C, 0x09, 0xCF, 0xC8, 0xB8, 0x4E, 0x60, 0xB5, 0x28, 0xDD, 0xD2, 0xE9, 0x8F, 0x73,
                                            0xB0, 0xDA, 0x1A, 0x24, 0x2A, 0x9A, 0xB6, 0x3D, 0x07, 0x6B, 0x66, 0x2B, 0xBE, 0x79, 0xEA, 0xBF };
    toWordArray(yBytes, y, SIZE);
    
    word X_res[SIZE], Y_res[SIZE], Z_res[SIZE];

    pointMultiply(k, g_x_mont, g_y_mont, one_mont, X_res, Y_res, Z_res);
    toCartesian(X_res, Y_res, Z_res, X_res, Y_res);

    assert(equalWordArrays(x, X_res, SIZE));
    assert(equalWordArrays(y, Y_res, SIZE));
    printf("Test %u - Online test point multiplication passed.\n", (*nbTest)++);
}

void test_ecdh(uint32_t *nbTest) {
    word scalar1[SIZE], scalar2[SIZE];
    uint8_t x1[SIZE * sizeof(word)], x2[SIZE * sizeof(word)],
            y1[SIZE * sizeof(word)], y2[SIZE * sizeof(word)];

    ECDHGenerateRandomSample(scalar1, x1, y1);
    ECDHGenerateRandomSample(scalar2, x2, y2);

    ECDHPointMultiply(scalar1, x2, y2, x2, y2);
    ECDHPointMultiply(scalar2, x1, y1, x1, y1);

    assert(equalByteArrays(x1, x2, SIZE * sizeof(word)));
    assert(equalByteArrays(y1, y2, SIZE * sizeof(word)));
    printf("Test %u - Test ECDH passed.\n", (*nbTest)++);
}

void test_sha3(uint32_t *nbTest) {
    word input1[2] = { 1, 5 };
    word input2[2] = { 2, 5 };
    word output1[256 / 8 / sizeof(word)];
    word output2[256 / 8 / sizeof(word)];

    sha3_HashBuffer(256, SHA3_FLAGS_NONE, input1, 2 * sizeof(word), output1, 256 / 8);
    sha3_HashBuffer(256, SHA3_FLAGS_NONE, input2, 2 * sizeof(word), output2, 256 / 8);
    
    assert(!equalWordArrays(output1, output2, SIZE));
    printf("Test %u - SHA3 passed.\n", (*nbTest)++);
}

void test_ec_arithmetic(uint32_t *nbTest) {
    test_mont_parameters(nbTest);
    test_ec_curve(nbTest);
    test_generator(nbTest);
    test_cartesian(nbTest);
    test_jacobian_cartesian(nbTest);
    test_montMul_zero(nbTest);
    test_pointDouble(nbTest);
    test_pointAdd(nbTest);
    test_pointDoubleAddConsistency(nbTest);
    test_multiplyComm(nbTest);
    test_curveOrder(nbTest);
    test_online(nbTest);
    test_ecdh(nbTest);
    test_sha3(nbTest);
}