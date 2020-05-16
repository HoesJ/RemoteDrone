#include "./../../include/general/compare_arrays.h"

/**
 * Return 1 if the arrays are equal, 0 otherwise.
 */
uint8_t equalWordArrays(const word *ar1, const word *ar2, size_t size) {
    size_t i;
    
    for (i = 0; i < size; i++)
        if (ar1[i] != ar2[i])
            return 0;
    
    return 1;
}

/**
 * Return 1 if the arrays are equal, 0 otherwise.
 */
uint8_t equalByteArrays(const void *ar1, const void *ar2, size_t size) {
    size_t i;
    
    for (i = 0; i < size; i++)
        if (((uint8_t*)ar1)[i] != ((uint8_t*)ar2)[i])
            return 0;
    
    return 1;
}

/**
 * Return 1 if array is zero, 0 otherwise.
 */
uint8_t isZero(const uint8_t* arr, size_t nbBytes) {
	size_t i;

	for (i = 0; i < nbBytes; i++)
		if (arr[i] != 0)
			return 0;
	return 1;
}