#include "./params.h"

#ifndef MOD_ARITHMETIC_H_
#define MOD_ARITHMETIC_H_

/**
 * Calculates res = (a + b) mod N.
 * - a and b represent operands, N is the modulus.
 * - a and b are both less than N.
 * - a, b, N and res represent large integers stored in word arrays of
 *   SIZE elements.
 * - arrays are composed with Little-Endian, meaning the least significant
 *   word comes first.
 */
void mod_add(const word *a, const word *b, const word *N, word *res);

/**
 * Calculates res = (a - b) mod N.
 * - a and b represent operands, N is the modulus.
 * - a and b are both less than N.
 * - a, b, N and res represent large integers stored in word arrays of
 *   SIZE elements.
 * - arrays are composed with Little-Endian, meaning the least significant
 *   word comes first.
 */
void mod_sub(const word *a, const word *b, const word *N, word *res);

/**
 * Calculates res = a * b * r^(-1) mod n.
 * a, b, n, n_prime represent operands of SIZE elements.
 * The result is stored in res which has SIZE elements.
 */
void montMul(const word *a, const word *b, const word *n, const word *n_prime, word *res);

/**
 * Calculates x^(-1) mod p.
 * x, p, N represent operands of SIZE elements.
 * inv has SIZE element.
 */
void mod_inv(const word *x, const word *p, word *inv);

#endif