#include "./../include/crt_drbg.h"

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

	uint8_t randomBuffer[1024];
	getRandomBytes(1024, randomBuffer);
	getRandomBytes(1024, randomBuffer);


#endif
}