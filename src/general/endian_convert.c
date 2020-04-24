#include "./../../include/general/endian_convert.h"

/**
 * Explicitly convert the endianness of the given array. Both data and result array
 * can be the same.
 */
void convertEndianness(const void *array, void *res, size_t length) {
	size_t currentByte;
	uint8_t tmp;

	for (currentByte = 0; currentByte < length / 2; currentByte++) {
		tmp = ((uint8_t*)array)[currentByte];
		((uint8_t*)res)[currentByte] = ((uint8_t*)array)[length - currentByte - 1];
		((uint8_t*)res)[length - currentByte - 1] = tmp;
	}
}

/**
 * Convert the given word array to little endian byte representation. The given data and
 * result array must not be the same.
 */
void toLittleEndian(const word *array, void *res, size_t nbWords) {
    size_t currentWord;
    uint8_t currentByte;

    for (currentWord = 0; currentWord < nbWords; currentWord++) {
        for (currentByte = 0; currentByte < sizeof(word); currentByte++)
            ((uint8_t*)res)[sizeof(word) * currentWord + currentByte] = (uint8_t)(array[currentWord] >> (8 * currentByte));
    }
}

/**
 * Convert the given little endian byte array to word representation. The given data and
 * result array must not be the same.
 */
void toWordArray(const void *array, word *res, size_t nbWords) {
    size_t currentWord;
    uint8_t currentByte;

    for (currentWord = 0; currentWord < nbWords; currentWord++) {
        res[currentWord] = 0;
        for (currentByte = 0; currentByte < sizeof(word); currentByte++)
            res[currentWord] |= ((word)((uint8_t*)array)[sizeof(word) * currentWord + currentByte]) << (8 * currentByte);
    }
}

/**
 * Convert the given number to little endian byte representation.
 */
void numToLittleEndian(const uint32_t number, void *res) {
    uint8_t currentByte;

    for (currentByte = 0; currentByte < sizeof(uint32_t); currentByte++)
        ((uint8_t*)res)[currentByte] = (uint8_t)(number >> (8 * currentByte));
}

/**
 * Convert the given little endian byte array to a number.
 */
uint32_t littleEndianToNum(const void *array, uint8_t nbBytes) {
    uint8_t currentByte;
    uint32_t res = 0;

    for (currentByte = 0; currentByte < nbBytes; currentByte++)
        res |= ((uint32_t)((uint8_t*)array)[currentByte]) << (8 * currentByte);
    
    return res;
}