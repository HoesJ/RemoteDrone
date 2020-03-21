#include "./../include/compare_arrays.h"

/**
 * Return 1 if the arrays are equal, 0 otherwise.
 */
word compareArrays(const word *ar1, const word *ar2, word size) {
    word i;
    
    for (i = 0; i < size; i++)
        if (ar1[i] != ar2[i])
            return 0;
    
    return 1;
}