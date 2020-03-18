#include "./../include/compare_arrays.h"

word compareArrays(const word *ar1, const word *ar2) {
    word i;
    
    for (i = 0; i < SIZE; i++)
        if (ar1[i] != ar2[i])
            return 0;
    
    return 1;
}