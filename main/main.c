#include "./../include/test.h"

int main(int argc, char const *argv[])
{
	word testctr;
	testctr = 0;

	test_arithmetic(&testctr);
	test_aegis(&testctr);
	//test_aes(&testctr);

    return 0;
}