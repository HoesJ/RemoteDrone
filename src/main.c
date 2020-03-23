#include "./../include/crt_drbg.h"
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
	test_ecdsa(&testctr);

	return 0;

#else

	word randomBuffer[12 / 4];
	getRandomBytes(12, randomBuffer);
	printNumber(randomBuffer, 12 / 4);
	getRandomBytes(12, randomBuffer);
	printNumber(randomBuffer, 12 / 4);

	return 0;

#endif
}