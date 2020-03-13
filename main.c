#include "arithmetic.h"

int main(int argc, char const *argv[])
{
    word a[SIZE];
    word b[SIZE];
    word N[SIZE];
    word res[SIZE + 1];

    mod_add(a, b, N, res);
    
    printf("%u", res[0]);
    return 0;
}