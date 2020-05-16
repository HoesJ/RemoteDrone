#include <stdint.h>

#ifndef WORD_SIZE_H_
#define WORD_SIZE_H_

/* Optimal word sizes for certain architecture.
   Can be 8, 16, 32 or 64 bits. */

#ifdef SIZE_8
#define BITS 8
#define SIZE 32
typedef int8_t   signed_word;
typedef uint8_t  word;
typedef uint16_t double_word;
#endif

#ifdef SIZE_16
#define BITS 16
#define SIZE 16
typedef int16_t   signed_word;
typedef uint16_t  word;
typedef uint32_t  double_word;
#endif

#ifdef SIZE_32
#define BITS 32
#define SIZE 8
typedef int32_t   signed_word;
typedef uint32_t  word;
typedef uint64_t  double_word;
#endif

#ifdef SIZE_64
#define BITS 64
#define SIZE 4
typedef int64_t     signed_word;
typedef uint64_t    word;
typedef __uint128_t double_word;
#endif

#endif