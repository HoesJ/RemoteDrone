#include "params.h"

#ifndef ARITHMETIC_H_
#define ARITHMETIC_H_

/* Calculates res = (a + b) mod N.
   - a and b represent operands, N is the modulus.
   - a and b are both less than N.
   - a, b, N and res represent large integers stored in word arrays of
     `length` elements.
   - arrays are composed with Little-Endian, meaning the least significant
     word comes first. */
void mod_add(volatile word *a, word *b, word *N, volatile word *res, word length);

/* Calculates res = (a - b) mod N.
   - a and b represent operands, N is the modulus.
   - a and b are both less than N.
   - a, b, N and res represent large integers stored in word arrays of
     `length` elements.
   - arrays are composed with Little-Endian, meaning the least significant
     word comes first. */
void mod_sub(word *a, word *b, word *N, word *res, word length);

/* Calculates res = a * b * r^(-1) mod n.
   a, b, n, n_prime represent operands of size elements.
   res has (size + 1) elements. */
void montMul(word *a, word *b, word *n, word *n_prime, word *res, word size);

#endif