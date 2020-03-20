#include "./../../include/test.h"
#include "./../../include/ec_parameters.h"
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
    assert(isOnCurve(g_x_mont, g_y_mont));
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

void runTests() {
    word nbTest = 1;

    test_mont_parameters(&nbTest);
    test_ec_curve(&nbTest);
    test_generator(&nbTest);
    test_cartesian(&nbTest);
    test_jacobian_cartesian(&nbTest);
    test_montMul_zero(&nbTest);
}