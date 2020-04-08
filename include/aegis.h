#include "params.h"
#include "aes.h"
#include "mod_arithmetic.h"
#include "logical_operations.h"
#include "compare_arrays.h"

#ifndef AEGIS_H_
#define AEGIS_H_

/**
 * The context for the AEGIS encryption system.
 * This context holds the state, key and IV which
 * makes it ease to carry over functions.
 */
struct AEGIS_ctx {
	uint8_t *key;
	uint8_t *iv;
	uint8_t state[80];
};

/**
 * Creates the AEGIS context required for the encryption
 * or decryption. It initialises the key and an empty
 * state array.
 */
void init_AEGIS_ctx(struct AEGIS_ctx *ctx, uint8_t *key);

/**
 * Creates the AEGIS context required for the encryption
 * or decryption. It initialises the key, IV and an empty
 * state array. The IV can be reset later with "setIV".
 */
void init_AEGIS_ctx_IV(struct AEGIS_ctx *ctx, uint8_t *key, uint8_t *iv);

/**
 * Sets the IV of the AEGIS encryption.
 */
void setIV(struct AEGIS_ctx *ctx, uint8_t *iv);

/**
 * Performs the full AEGIS decryption on the initialized
 * context, given associated data and ciphertext
 * - ctx should have a key and fresh iv
 * - ad has adlen * AES_BLOCK_NUMEL elements
 * - plain has plainlen * AES_BLOCK_NUMEL elements
 * - cipher has plainlen * AES_BLOCK_NUMEL elements
 * - tag has 1 * AES_BLOCK_NUMEL elements
 */
void aegisEncrypt(struct AEGIS_ctx* ctx, uint8_t* ad, word adlen, uint8_t* plain, word plainlen, uint8_t* cipher, uint8_t* tag);

/**
 * Performs the full AEGIS decryption on the initialized
 * context, given associated data and ciphertext
 * - ctx should have a key and fresh iv
 * - ad has adlen * AES_BLOCK_NUMEL elements
 * - cipher has cipherlen * AES_BLOCK_NUMEL elements
 * - plain has plainlen * AES_BLOCK_NUMEL elements
 * - tag has 1 * AES_BLOCK_NUMEL elements
 */
void aegisDecrypt(struct AEGIS_ctx* ctx, uint8_t* ad, word adlen, uint8_t* cipher, word cipherlen, uint8_t* plain, uint8_t* tag);

/**
 * Encapsulates the AEGIS complexities to encrypt a message.
 * The message buffer should have the associated data at the beginning.
 * The plaintext to be encrypted should be after the associated data.
 * After both there should be room for a 16 byte MAC.
 * The ciphertext will replace the plaintext.
 *
 * adLen and plainLen are amount of bytes
 * adLen can be not a multiple of 16 and will be padded
 * plainLen should be a multiple of 16
 */
void aegisEncryptMessage(struct AEGIS_ctx* ctx, uint8_t* message, word adLen, word plainLen);

/**
 * Encapsulates the AEGIS complexities to decrypt a message.
 * The message buffer should have the associated data at the beginning.
 * The ciphertext to be decrypted should be after the associated data.
 * The plaintext will replace the ciphertext.
 *
 * The MAC at the end will be checked, and its corectness returned.
 *
 * adLen and plainLen are amount of bytes
 * adLen can be not a multiple of 16 and will be padded
 * plainLen should be a multiple of 16
 */
word aegisDecryptMessage(struct AEGIS_ctx* ctx, uint8_t* message, word adLen, word cipherLen);

#endif