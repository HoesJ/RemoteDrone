#include "./../include/compare_arrays.h"

/**
 * Return 1 if the arrays are equal, 0 otherwise.
 */
word compareWordArrays(const word *ar1, const word *ar2, word size) {
    word i;
    
    for (i = 0; i < size; i++)
        if (ar1[i] != ar2[i])
            return 0;
    
    return 1;
}

/**
 * Return 1 if the arrays are equal, 0 otherwise.
 */
word compareByteArrays(const void *ar1, const void *ar2, word size) {
    word i;
    
    for (i = 0; i < size; i++)
        if (((uint8_t*)ar1)[i] != ((uint8_t*)ar2)[i])
            return 0;
    
    return 1;
}