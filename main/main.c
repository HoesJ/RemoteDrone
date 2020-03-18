#include "./../include/mod_arithmetic.h"

int main(int argc, char const *argv[])
{
    word x[SIZE] = {0};
    word p[SIZE] = {0};
    word inv[SIZE] = {0};

    x[0] = 5;
    p[0] = 17;

    mod_inv(x, p, inv);

    printf("%u", inv[0]);
    return 0;
}