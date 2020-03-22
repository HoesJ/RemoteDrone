#include "params.h"

#ifndef ECDSA_H_
#define ECDSA_H_

/* Sign the given message. */
void ecdsaSign(const uint8_t *message, const word length, const word *privateKey, word *r, word *s);

#endif