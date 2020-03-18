#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef PARAMS_H_
#define PARAMS_H_

typedef int32_t  signed_word;
typedef uint32_t word;
typedef uint64_t double_word;

#define BITS 32

#ifdef MOD_TESTS
#define SIZE 32
#else
#define SIZE 8
#endif

#endif