#ifndef _AEGIS_H_
#define _AEGIS_H_

#include "params.h"
#include "aes.h"

/*
	The context for the AEGIS encryption system.
	This context holds the state, key and IV which
	makes it ease to carry over functions.
*/
struct AEGIS_ctx {
	uint8_t*	key;
	uint8_t*	iv;
	uint8_t	    state[80];
};

/*
	Creates the AEGIS context required for the encryption
	or decryption. It initialises the key and an empty
	state array. key is of size AES_BLOCK_NUMEL
*/
void createAegisContext(struct AEGIS_ctx* ctx, uint8_t* key);

/*
	Creates the AEGIS context required for the encryption
	or decryption. It initialises the key, IV and an empty
	state array. The IV can be reset later with "setIV"
	key and iv are of size AES_BLOCK_NUMEL
*/
void createAegisContextIV(struct AEGIS_ctx* ctx, uint8_t* key, uint8_t* iv);

/*
	Sets the IV of the AEGIS encryption.
	IV should have AES_BLOCK_NUMEL elements
*/
void setIV(struct AEGIS_ctx* ctx, uint8_t* iv);

/*
	Performs the full AEGIS decryption on the initialized
	context, given associated data and ciphertext
		- ctx should have a key and fresh iv
		- ad has ad_size * AES_BLOCK_NUMEL elements
		- plain has plain_size * AES_BLOCK_NUMEL elements
		- cipher has plain_size * AES_BLOCK_NUMEL elements
		- tag has 1 * AES_BLOCK_NUMEL elements
*/
void aegisEncrypt(struct AEGIS_ctx* ctx, uint8_t* ad, word ad_size, uint8_t* plain, word plain_size, uint8_t* cipher, uint8_t* tag);

/*
	Performs the full AEGIS decryption on the initialized
	context, given associated data and ciphertext
		- ctx should have a key and fresh iv
		- ad has ad_size * AES_BLOCK_NUMEL elements
		- cipher has cipher_size * AES_BLOCK_NUMEL elements
		- plain has plain_size * AES_BLOCK_NUMEL elements
		- tag has 1 * AES_BLOCK_NUMEL elements
*/
void aegisDecrypt(struct AEGIS_ctx* ctx, uint8_t* ad, word ad_size, uint8_t* cipher, word cipher_size, uint8_t* plain, uint8_t* tag);


#endif //_AEGIS_H_