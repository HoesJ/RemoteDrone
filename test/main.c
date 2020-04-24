#include "./../include/main/test.h"

int main(int argc, char const *argv[])
{
	word testctr = 1;

	test_mod_arithmetic(&testctr);
	test_aes(&testctr);
	test_aegis(&testctr);
	test_endianness(&testctr);
	test_ec_arithmetic(&testctr);
	test_ecdsa(&testctr);
	test_pipe(&testctr);

	return 0;
}