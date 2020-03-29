#include "./../include/compare_arrays.h"

/**
 * Return 1 if the arrays are equal, 0 otherwise.
 */
word equalWordArrays(const word *ar1, const word *ar2, size_t size) {
    size_t i;
    
    for (i = 0; i < size; i++)
        if (ar1[i] != ar2[i])
            return 0;
    
    return 1;
}

/**
 * Return 1 if the arrays are equal, 0 otherwise.
 */
word equalByteArrays(const void *ar1, const void *ar2, size_t size) {
    size_t i;
    
    for (i = 0; i < size; i++)
        if (((uint8_t*)ar1)[i] != ((uint8_t*)ar2)[i])
            return 0;
    
    return 1;
}

/**
 * Return 1 if array is zero, 0 otherwise.
 */
word isZero(const uint8_t* arr, word bytes) {
	word i;

	for (i = 0; i < bytes; i++)
		if (arr[i] != 0)
			return 0;
	return 1;
}