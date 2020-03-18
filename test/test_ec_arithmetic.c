#include <assert.h>
#include "./../include/ec_parameters.h"
#include "./../include/mod_arithmetic.h"
#include "./../include/compare_arrays.h"

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

    word b_mont[SIZE + 1], c_mont[SIZE + 1], res[SIZE + 1];
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

int main(int argc, char const *argv[])
{
    word nbTest = 1;

    test_mont_parameters(&nbTest);
    test_ec_curve(&nbTest);

    return 0;
}