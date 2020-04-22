#include "params.h"

#ifndef COMPARE_ARRAYS_H_
#define COMPARE_ARRAYS_H_

/**
 * Return 1 if the arrays are equal, 0 otherwise.
 */
word equalWordArrays(const word *ar1, const word *ar2, size_t size);

/**
 * Return 1 if the arrays are equal, 0 otherwise.
 */
word equalByteArrays(const void *ar1, const void *ar2, size_t size);

/**
 * Return 1 if array is zero, 0 otherwise.
 */
word isZero(const uint8_t* arr, word bytes);

#endif