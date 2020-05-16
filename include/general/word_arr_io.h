#include "params.h"
#include "endian_convert.h"

#ifndef WORD_ARR_IO_H_
#define WORD_ARR_IO_H_

/**
 * Write the given word array to a file in little endian representation.
 */
void writeWordArrToFile(word *arr, const char *filePath);

/**
 * Read a little endian represented word array from a file.
 */
void readWordArrFromFile(word *arr, const char *filePath);

#endif