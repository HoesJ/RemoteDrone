#include "params.h"

#ifndef ENDIAN_CONVERT_H_
#define ENDIAN_CONVERT_H_

/**
 * Explicitly convert the endianness of the given array. Both data and result array
 * can be the same.
 */
void convertEndianness(const void *array, void *res, size_t length);

/**
 * Convert the given word array to little endian byte representation. The given data and
 * result array must not be the same.
 */
void toLittleEndian(const word *array, void *res, size_t nbWords);

/**
 * Convert the given little endian byte array to word representation. The given data and
 * result array must not be the same.
 */
void toWordArray(const void *array, word *res, size_t nbWords);

/**
 * Convert the given number to little endian byte representation.
 */
void numToLittleEndian(const uint32_t number, void *res);

/**
 * Convert the given little endian byte array to a number.
 */
uint32_t littleEndianToNum(const void *array, uint8_t nbBytes);

#endif