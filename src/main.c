#include "./../include/print_number.h"
#include "./../include/test.h"

int main(int argc, char const *argv[])
{
    /* word mont[SIZE + 1];
    montMul(one, r_2, p, p_prime, mont);
    printNumber(mont, SIZE); */

	word testctr;
	testctr = 0;

	test_arithmetic(&testctr);
	test_aegis(&testctr);
	test_ec_arithmetic(&testctr);
	/* test_aes(&testctr); */

    return 0;
}