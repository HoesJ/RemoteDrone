#include "params.h"

#ifndef ECDSA_H_
#define ECDSA_H_

/**
 * Generate a public/private key pair. The public key coordinates are represented
 * as numbers in the Montgomery domain.
 */
void ecdsaGenerateKeyPair(word *privateKey, word *pkx_mont, word pky_mont);

/* Sign the given message. */
void ecdsaSign(const uint8_t *message, const word length, const word *privateKey, word *r, word *s);

/* Check the signature on the the given message. */
word ecdsaCheck(const uint8_t *message, const word length, const word *pkx_mont, const word *pky_mont, const word *r, const word *s);

#endif