#include "./../include/arithmetic.h"

int main(int argc, char const *argv[])
{
    word a[SIZE] = {0};
    word b[SIZE] = {0};
    word N[SIZE] = {0};
    word res[SIZE + 1] = {0};
    mod_add(a, b, N, res);
    
    printf("%u", res[0]);
    return 0;
}