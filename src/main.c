#include "./../include/test.h"
#include "./../include/print_number.h"
#include "./../include/mod_arithmetic.h"

#ifdef TESTS

int main(int argc, char const *argv[])
{
    runTests();
    return 0;
}

#else

int main(int argc, char const *argv[])
{
    word mont[SIZE + 1];
    montMul(one, r_2, p, p_prime, mont);
    printNumber(mont, SIZE);

    return 0;
}

#endif