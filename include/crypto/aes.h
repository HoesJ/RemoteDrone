#include "./../general/params.h"

#ifndef AES_H_
#define AES_H_

#define AES_BLOCKLEN   16   /* Block length in bytes AES is 128 bit block only */
#define AES_KEYLEN 	   16	/* Key length in bytes */
#define AES_KEYEXPSIZE 176

struct AES_ctx {
	uint8_t RoundKey[AES_KEYEXPSIZE];
};

void AES_init_ctx(struct AES_ctx *ctx, const uint8_t *key);
void AES_ECB_encrypt(const struct AES_ctx *ctx, uint8_t *buf);
void AESRound(uint8_t *out, uint8_t *in, uint8_t *key);

#endif