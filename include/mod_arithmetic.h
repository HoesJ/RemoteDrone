#include "params.h"

#ifndef MOD_ARITHMETIC_H_
#define MOD_ARITHMETIC_H_

/*
 * Calculates res = (a + b) and detects overflow.
 */
word add_overflow(const word *a, const word *b, word *res);

/* Calculates res = (a + b) mod N.
   - a and b represent operands, N is the modulus.
   - a and b are both less than N.
   - a, b, N and res represent large integers stored in word arrays of
     SIZE elements.
   - arrays are composed with Little-Endian, meaning the least significant
     word comes first. */
void mod_add(const word *a, const word *b, const word *N, word *res);

/* Calculates res = (a - b) mod N.
   - a and b represent operands, N is the modulus.
   - a and b are both less than N.
   - a, b, N and res represent large integers stored in word arrays of
     SIZE elements.
   - arrays are composed with Little-Endian, meaning the least significant
     word comes first. */
void mod_sub(const word *a, const word *b, const word *N, word *res);

/* Calculates res = a * b * r^(-1) mod n.
   a, b, n, n_prime represent operands of SIZE elements.
   res has (SIZE + 1) elements. */
void montMul(const word *a, const word *b, const word *n, const word *n_prime, word *res);

/*	Calculates x^(-1) mod p.
   	x, p, N represent operands of SIZE elements.
	  inv has SIZE element. */
void mod_inv(const word *x, const word *p, word *inv);

/* Calculates the XOR of two word arrays of both size elements.
	 Warning: This is a macro that needs you to declare i yourself!
*/
#define XOR128(res, a, b) {												\
((uint64_t*)(res))[0] = ((uint64_t*)(a))[0] ^ ((uint64_t*)(b))[0];		\
((uint64_t*)(res))[1] = ((uint64_t*)(a))[1] ^ ((uint64_t*)(b))[1];		\
}

/* Calculates the AND of two word arrays of both size elements.
	 Warning: This is a macro that needs you to declare i yourself!
*/
#define AND128(res, a, b) {												\
((uint64_t*)(res))[0] = ((uint64_t*)(a))[0] & ((uint64_t*)(b))[0];		\
((uint64_t*)(res))[1] = ((uint64_t*)(a))[1] & ((uint64_t*)(b))[1];		\
}

#endif