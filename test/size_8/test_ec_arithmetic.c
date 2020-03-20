#include "./../../include/test.h"
#include "./../../include/mod_arithmetic.h"
#include "./../../include/compare_arrays.h"
#include "./../../include/ec_arithmetic.h"

void test_mont_parameters(word *nbTest) {
    word one_mont[SIZE + 1], res[SIZE + 1];
    
    montMul(one, r_2, p, p_prime, one_mont);
    montMul(one_mont, one_mont, p, p_prime, res);
    montMul(res, one, p, p_prime, res);
    
    assert(compareArrays(res, one));
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
    
    montMul(b, r_2, p, p_prime, b_mont);
    montMul(c, r_2, p, p_prime, c_mont);

    montMul(b_mont, b_mont, p, p_prime, res);
    montMul(res, c_mont, p, p_prime, res);
    montMul(res, one, p, p_prime, res);
    
    assert(compareArrays(res, min_27));
    printf("Test %u - Curve parameters passed.\n", (*nbTest)++);
}

void test_generator(word *nbTest) {
    assert(isOnCurve(g_x, g_y));
    printf("Test %u - Generator on curve passed.\n", (*nbTest)++);
}

void test_cartesian(word *nbTest) {
    word x_res[SIZE], y_res[SIZE];

    toCartesian(g_x_mont, g_y_mont, one_mont, p, p_prime, x_res, y_res);

    assert(compareArrays(x_res, g_x));
    assert(compareArrays(y_res, g_y));
    printf("Test %u - Conversion to cartesian coordinates passed.\n", (*nbTest)++);
}

void test_jacobian_cartesian(word *nbTest) {
    word X[SIZE], Y[SIZE], Z[SIZE], x[SIZE], y[SIZE];

    toJacobian(g_x, g_y, p, p_prime, X, Y, Z);
    toCartesian(X, Y, Z, p, p_prime, x, y);

    assert(compareArrays(x, g_x));
    assert(compareArrays(y, g_y));
    printf("Test %u - Conversion to Jacobian/cartesian coordinates passed.\n", (*nbTest)++);
}

void test_montMul_zero(word *nbTest) {
    word res[SIZE];

    montMul(zero, zero, p, p_prime, res);

    assert(compareArrays(res, zero));
    printf("Test %u - Montgomery multiplication with zero coordinates passed.\n", (*nbTest)++);
}

void test_pointDouble(word *nbTest) {
    word X_res[SIZE], Y_res[SIZE], Z_res[SIZE], x_res[SIZE], y_res[SIZE];

    pointDouble(g_x_mont, g_y_mont, one_mont, p, p_prime, X_res, Y_res, Z_res);
    toCartesian(X_res, Y_res, Z_res, p, p_prime, x_res, y_res);

    assert(isOnCurve(x_res, y_res));
    printf("Test %u - Point doubling on curve passed.\n", (*nbTest)++);
}

void test_pointAdd(word *nbTest) {
    word X_res[SIZE], Y_res[SIZE], Z_res[SIZE], x_res[SIZE], y_res[SIZE];

    pointDouble(g_x_mont, g_y_mont, one_mont, p, p_prime, X_res, Y_res, Z_res);
    pointAdd(g_x_mont, g_y_mont, one_mont, X_res, Y_res, Z_res, p, p_prime, X_res, Y_res, Z_res);
    toCartesian(X_res, Y_res, Z_res, p, p_prime, x_res, y_res);

    assert(isOnCurve(x_res, y_res));
    printf("Test %u - Point addition on curve passed.\n", (*nbTest)++);
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
         x_res[SIZE],     y_res[SIZE];

    pointMultiply(scalar1, g_x_mont, g_y_mont, one_mont, p, p_prime, X_res_tmp, Y_res_tmp, Z_res_tmp);
    pointMultiply(scalar2, X_res_tmp, Y_res_tmp, Z_res_tmp, p, p_prime, X_res1, Y_res1, Z_res1);

    pointMultiply(scalar2, g_x_mont, g_y_mont, one_mont, p, p_prime, X_res_tmp, Y_res_tmp, Z_res_tmp);
    pointMultiply(scalar1, X_res_tmp, Y_res_tmp, Z_res_tmp, p, p_prime, X_res2, Y_res2, Z_res2);

    assert(compareArrays(X_res1, X_res2));
    assert(compareArrays(Y_res1, Y_res2));
    assert(compareArrays(Z_res1, Z_res2));

    toCartesian(X_res1, Y_res1, Z_res1, p, p_prime, x_res, y_res);

    assert(isOnCurve(x_res, y_res));
    printf("Test %u - Commutative point multiplication passed.\n", (*nbTest)++);
}

void runTests() {
    word nbTest = 1;

    test_mont_parameters(&nbTest);
    test_ec_curve(&nbTest);
    test_generator(&nbTest);
    test_cartesian(&nbTest);
    test_jacobian_cartesian(&nbTest);
    test_montMul_zero(&nbTest);
    test_pointDouble(&nbTest);
    test_pointAdd(&nbTest);
    test_multiplyComm(&nbTest);
}