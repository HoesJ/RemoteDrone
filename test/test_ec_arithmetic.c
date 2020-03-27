#include "./../include/test.h"

void test_mont_parameters(word *nbTest) {
    word one_mont[SIZE], res[SIZE];
    
    /* Test p. */
    montMul(one, rp_2, p, p_prime, one_mont);
    montMul(one_mont, one_mont, p, p_prime, res);
    montMul(res, one, p, p_prime, res);
    
    assert(compareWordArrays(res, one, SIZE));
    printf("Test %u - Montgomery parameters passed.\n", (*nbTest)++);

    /* Test n. */
    montMul(one, rn_2, n, n_prime, one_mont);
    montMul(one_mont, one_mont, n, n_prime, res);
    montMul(res, one, n, n_prime, res);
    
    assert(compareWordArrays(res, one, SIZE));
    printf("Test %u - Montgomery parameters passed.\n", (*nbTest)++);
}

void test_ec_curve(word *nbTest) {
    /* Curve should satisfy b * b * c = -27 (mod p). */

    word b_mont[SIZE], c_mont[SIZE], res[SIZE];
    word min_27[SIZE] = { 0xFFFFFFE4,
                          0xFFFFFFFF,
                          0xFFFFFFFF,
                          0x00000000,
                          0x00000000,
                          0x00000000,
                          0x00000001,
                          0xFFFFFFFF };
    
    montMul(b, rp_2, p, p_prime, b_mont);
    montMul(c, rp_2, p, p_prime, c_mont);

    montMul(b_mont, b_mont, p, p_prime, res);
    montMul(res, c_mont, p, p_prime, res);
    montMul(res, one, p, p_prime, res);
    
    assert(compareWordArrays(res, min_27, SIZE));
    printf("Test %u - Curve parameters passed.\n", (*nbTest)++);
}

void test_generator(word *nbTest) {
    assert(isOnCurve(g_x, g_y));
    printf("Test %u - Generator on curve passed.\n", (*nbTest)++);
}

void test_cartesian(word *nbTest) {
    word x_res[SIZE], y_res[SIZE];

    toCartesian(g_x_mont, g_y_mont, one_mont, x_res, y_res);

    assert(compareWordArrays(x_res, g_x, SIZE));
    assert(compareWordArrays(y_res, g_y, SIZE));
    printf("Test %u - Conversion to cartesian coordinates passed.\n", (*nbTest)++);
}

void test_jacobian_cartesian(word *nbTest) {
    word X[SIZE], Y[SIZE], Z[SIZE], x[SIZE], y[SIZE];

    toJacobian(g_x, g_y, X, Y, Z);
    toCartesian(X, Y, Z, x, y);

    assert(compareWordArrays(x, g_x, SIZE));
    assert(compareWordArrays(y, g_y, SIZE));
    printf("Test %u - Conversion to Jacobian/cartesian coordinates passed.\n", (*nbTest)++);
}

void test_montMul_zero(word *nbTest) {
    word res[SIZE];

    montMul(zero, zero, p, p_prime, res);

    assert(compareWordArrays(res, zero, SIZE));
    printf("Test %u - Montgomery multiplication with zero coordinates passed.\n", (*nbTest)++);
}

void test_pointDouble(word *nbTest) {
    word X_res[SIZE], Y_res[SIZE], Z_res[SIZE], x_res[SIZE], y_res[SIZE];

    pointDouble(g_x_mont, g_y_mont, one_mont, X_res, Y_res, Z_res);
    toCartesian(X_res, Y_res, Z_res, x_res, y_res);

    assert(isOnCurve(x_res, y_res));
    printf("Test %u - Point doubling on curve passed.\n", (*nbTest)++);
}

void test_pointAdd(word *nbTest) {
    word X_res[SIZE], Y_res[SIZE], Z_res[SIZE], x_res[SIZE], y_res[SIZE];

    pointDouble(g_x_mont, g_y_mont, one_mont, X_res, Y_res, Z_res);
    pointAdd(g_x_mont, g_y_mont, one_mont, X_res, Y_res, Z_res, X_res, Y_res, Z_res);
    toCartesian(X_res, Y_res, Z_res, x_res, y_res);

    assert(isOnCurve(x_res, y_res));
    printf("Test %u - Point addition on curve passed.\n", (*nbTest)++);
}

void test_pointDoubleAddConsistency(word *nbTest) {
    word X_res1[SIZE], Y_res1[SIZE], Z_res1[SIZE], x_res1[SIZE], y_res1[SIZE];
    word X_res2[SIZE], Y_res2[SIZE], Z_res2[SIZE], x_res2[SIZE], y_res2[SIZE];

    pointDouble(g_x_mont, g_y_mont, one_mont, X_res1, Y_res1, Z_res1);
    toCartesian(X_res1, Y_res1, Z_res1, x_res1, y_res1);

    pointAdd(g_x_mont, g_y_mont, one_mont, g_x_mont, g_y_mont, one_mont, X_res2, Y_res2, Z_res2);
    toCartesian(X_res2, Y_res2, Z_res2, x_res2, y_res2);

    assert(compareWordArrays(x_res1, x_res2, SIZE));
    assert(compareWordArrays(y_res1, y_res2, SIZE));
    printf("Test %u - Point doubling/addition consistency passed.\n", (*nbTest)++);
}

void test_multiplyComm(word *nbTest) {
    word scalar1[SIZE] = { 0x04578754,
                           0x00AB4DFE,
                           0x04578754,
                           0x00000000,
                           0x010FDEFC,
                           0x00AB4DFE,
                           0x04578754,
                           0x00AB4DF4 };
    
    word scalar2[SIZE] = { 0x04574710,
                           0x00AB4DFE,
                           0x04587754,
                           0x0FFFE000,
                           0x010FDEFC,
                           0x00AB4DFE,
                           0x04578754,
                           0x00AB4DF4 };
    
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
    
    assert(compareWordArrays(x_res1, x_res2, SIZE));
    assert(compareWordArrays(y_res1, y_res2, SIZE));

    assert(isOnCurve(x_res1, y_res1));
    printf("Test %u - Commutative point multiplication passed.\n", (*nbTest)++);
}

void test_curveOrder(word *nbTest) {
    word X[SIZE], Y[SIZE], Z[SIZE];

    pointMultiply(n, g_x_mont, g_y_mont, one_mont, X, Y, Z);

    assert(compareWordArrays(X, zero, SIZE));
    assert(compareWordArrays(Y, zero, SIZE));

    printf("Test %u - High order subgroup passed.\n", (*nbTest)++);
}

void test_sha3(word *nbTest) {
    word input1[2] = { 1, 5 };
    word input2[2] = { 2, 5 };
    word output1[256 / 8 / sizeof(word)];
    word output2[256 / 8 / sizeof(word)];

    sha3_HashBuffer(256, SHA3_FLAGS_NONE, input1, 2 * sizeof(word), output1, 256 / 8);
    sha3_HashBuffer(256, SHA3_FLAGS_NONE, input2, 2 * sizeof(word), output2, 256 / 8);
    
    assert(!compareWordArrays(output1, output2, SIZE));
    printf("Test %u - SHA3 passed.\n", (*nbTest)++);
}

void test_ec_arithmetic(word *nbTest) {
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
    test_sha3(nbTest);
}