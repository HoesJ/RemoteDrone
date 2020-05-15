#include "params.h"

#ifndef PRINT_NUMBER_H_
#define PRINT_NUMBER_H_

/**
 * Print the given number in hexadecimal form.
 */
void printNumber(const word *number, size_t size);

/**
 * Print the given number in little endian hexadecimal form.
 */
void printBytes(const word *number, size_t size, const char *var);

#endif