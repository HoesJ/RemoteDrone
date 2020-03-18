#include "./../include/print_number.h"
#include "./../include/ec_parameters.h"
#include "./../include/mod_arithmetic.h"

int main(int argc, char const *argv[])
{
    word mont[SIZE + 1];
    montMul(a, r_2, p, p_prime, mont);
    printNumber(mont);
    return 0;
}