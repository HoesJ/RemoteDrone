#include "./../include/arithmetic.h"

int main(int argc, char const *argv[])
{
    word x[SIZE] = {0};
    word p[SIZE] = {0};
    word inv[SIZE] = {0};

    x[0] = 5;
    p[0] = 17;

    mod_inv(x, p, inv);

    printf("%u", p[0]);
    return 0;
}