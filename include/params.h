#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef PARAMS_H_
#define PARAMS_H_

typedef int32_t  signed_word;
typedef uint32_t word;
typedef uint64_t double_word;

extern const word ALL_ONE_MASK;
extern const word LEFT_ONE_MASK;

#define BITS 32

#ifdef MOD_TESTS
#define SIZE 32
#else
#define SIZE 8
#endif

extern const word p[SIZE];
extern const word p_prime[SIZE];
extern const word r_2[SIZE];
extern const word zero[SIZE];
extern const word one[SIZE];
extern const word one_mont[SIZE];
extern const word a[SIZE];
extern const word a_mont[SIZE];
extern const word b[SIZE];
extern const word b_mont[SIZE];
extern const word c[SIZE];
extern const word g_x[SIZE];
extern const word g_x_mont[SIZE];
extern const word g_y[SIZE];
extern const word g_y_mont[SIZE];

#endif