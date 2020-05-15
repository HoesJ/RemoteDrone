#include "params.h"

#ifndef PRINT_NUMBER_H_
#define PRINT_NUMBER_H_

/**
 * Print the given number in hexadecimal form.
 */
void printNumber(const word *number, size_t size);

/**
 * Print the given byte array in hexadecimal form.
 */
void printBytes(const word *arr, size_t size);

#endif