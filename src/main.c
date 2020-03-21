#include "./../include/print_number.h"

#ifdef TESTS
#include "./../include/test.h"
#endif


int main(int argc, char const *argv[])
{
#ifndef TESTS
	word testctr;
	testctr = 0;

	test_arithmetic(&testctr);
	test_aegis(&testctr);
	test_ec_arithmetic(&testctr);
	return 0;
#endif
	
	word mont[SIZE + 1];
	montMul(one, r_2, p, p_prime, mont);
	printNumber(mont, SIZE);

    return 0;
}