#include "./../include/print_number.h"

#ifdef TESTS
#include "./../include/test.h"
#endif


int main(int argc, char const *argv[])
{
#ifdef TESTS

	word testctr = 1;

	test_mod_arithmetic(&testctr);
	test_aes(&testctr);
	test_aegis(&testctr);
	test_ec_arithmetic(&testctr);

	return 0;

#else
	
	word mont[SIZE];
	montMul(one, r_2, p, p_prime, mont);
	printNumber(mont, SIZE);

    return 0;

#endif
}