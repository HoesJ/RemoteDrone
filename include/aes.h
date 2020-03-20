#ifndef _AES_H_
#define _AES_H_

#include "params.h"
#include "arithmetic.h"

void AESRound(uint8_t* out, uint8_t* in, uint8_t* key);

#endif //_AES_H_
