#include "./../include/test/test.h"

void t1(uint32_t* nb) {
	/* associated data: 0 bits plaintext: 128 bits */
	struct AEGIS_ctx ctx;
	uint8_t key[16] = { 0 };
	uint8_t iv[16] = { 0 };
	uint8_t plaintext[16] = { 0 };
	uint8_t ciphertext[16] = { 0 };
	uint8_t tag[16] = { 0 };

	uint8_t res_ciphertext[16] = { 0x95, 0x1b, 0x05, 0x0f, 0xa7, 0x2b, 0x1a, 0x2f, 0xc1, 0x6d, 0x2e, 0x1f, 0x01, 0xb0, 0x7d, 0x7e };
	uint8_t res_tag[16] = { 0xa7, 0xd2, 0xa9, 0x97, 0x73, 0x24, 0x95, 0x42, 0xf4, 0x22, 0x21, 0x7e, 0xe8, 0x88, 0xd5, 0xf1 };

	init_AEGIS_ctx_IV(&ctx, key, iv);
	aegisEncrypt(&ctx, iv, 0, plaintext, 16, ciphertext, tag);

	assert(equalWordArrays((word*)ciphertext, (word*)res_ciphertext, 4));
	assert(equalWordArrays((word*)tag, (word*)res_tag, 4));
	printf("Test %u - AEGIS passed.\n", (*nb)++);
}

void t2(uint32_t* nb) {
	/* associated data : 128 bits plaintext : 128 bits */
	struct AEGIS_ctx ctx;
	uint8_t key[16] = { 0 };
	uint8_t iv[16] = { 0 };
	uint8_t ad[16] = { 0 };
	uint8_t plaintext[16] = { 0 };
	uint8_t ciphertext[16] = { 0 };
	uint8_t tag[16] = { 0 };

	uint8_t res_ciphertext[16] = { 0x10, 0xb0, 0xde, 0xe6, 0x5a, 0x97, 0xd7, 0x51, 0x20, 0x5c, 0x12, 0x8a, 0x99, 0x24, 0x73, 0xa1 };
	uint8_t res_tag[16] = { 0x46, 0xdc, 0xb9, 0xee, 0x93, 0xc4, 0x6c, 0xf1, 0x37, 0x31, 0xd4, 0x1b, 0x96, 0x46, 0xc1, 0x31 };

	init_AEGIS_ctx_IV(&ctx, key, iv);
	aegisEncrypt(&ctx, ad, 16, plaintext, 16, ciphertext, tag);

	assert(equalWordArrays((word*)ciphertext, (word*)res_ciphertext, 4));
	assert(equalWordArrays((word*)tag, (word*)res_tag, 4));
	printf("Test %u - AEGIS passed.\n", (*nb)++);
}

void t3(uint32_t* nb) {
	/* associated data: 128 bits plaintext: 128 bits */
	struct AEGIS_ctx ctx;
	uint8_t key[16] = { 0 };
	uint8_t iv[16] = { 0 };	
	uint8_t ad[16] = { 0x00, 0x01, 0x02, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	uint8_t plaintext[16] = { 0 };
	uint8_t ciphertext[16] = { 0 };
	uint8_t tag[16] = { 0 };

	uint8_t res_ciphertext[16] = { 0x2b, 0x78, 0xf5, 0xc1, 0x61, 0x8d, 0xa3, 0x9a, 0xfb, 0xb2, 0x92, 0x0f, 0x5d, 0xae, 0x02, 0xb0 };
	uint8_t res_tag[16] = { 0xa7, 0xa0, 0x08, 0xc6, 0x96, 0xf1, 0x45, 0xdc, 0x46, 0x65, 0xe2, 0xa4, 0x3f, 0x86, 0xaa, 0xe3 };
 
 	key[1] = 0x01; iv[2] = 0x02;

	init_AEGIS_ctx_IV(&ctx, key, iv);
	aegisEncrypt(&ctx, ad, 16, plaintext, 16, ciphertext, tag);

	assert(equalWordArrays((word*)ciphertext, (word*)res_ciphertext, 4));
	assert(equalWordArrays((word*)tag, (word*)res_tag, 4));
	printf("Test %u - AEGIS passed.\n", (*nb)++);
}

void t4(uint32_t* nb) {
	/* associated data: 128 bits plaintext: 256 bits */
	struct AEGIS_ctx ctx;
	uint8_t key[16] = { 0 }; 
	uint8_t iv[16] = { 0 };
	uint8_t ad[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	uint8_t plaintext[32] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
								0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };
	uint8_t ciphertext[32] = { 0 };
	uint8_t tag[16] = { 0 };

	uint8_t res_ciphertext[32] = { 0xe0, 0x8e, 0xc1, 0x06, 0x85, 0xd6, 0x3c, 0x73, 0x64, 0xec, 0xa7, 0x8f, 0xf6, 0xe1,0xa1, 0xdd, 
									0xfd, 0xfc, 0x15, 0xd5, 0x31, 0x1a, 0x7f, 0x29, 0x88, 0xa0, 0x47, 0x1a, 0x13, 0x97, 0x3f, 0xd7 };
	uint8_t res_tag[16] = { 0x3e, 0x35, 0xbb, 0xb5, 0xd1, 0xf0, 0x84, 0x36, 0x71, 0x18, 0xa9, 0x90, 0x20, 0x90, 0xd8, 0x61 };

	key[0] = 0x10;  key[1] = 0x01; iv[0] = 0x10;  iv[2] = 0x02;

	init_AEGIS_ctx_IV(&ctx, key, iv);
	aegisEncrypt(&ctx, ad, 16, plaintext, 32, ciphertext, tag);

	assert(equalWordArrays((word*)ciphertext, (word*)res_ciphertext, 4));
	assert(equalWordArrays((word*)tag, (word*)res_tag, 4));

	aegisDecrypt(&ctx, ad, 16, ciphertext, 32, res_ciphertext, tag);

	assert(equalWordArrays((word*)plaintext, (word*)res_ciphertext, 4));

	printf("Test %u - AEGIS passed.\n", (*nb)++);
}

void t5(uint32_t* nb) {
	/* associated data: 128 bits plaintext: 84 bytes */
	struct AEGIS_ctx ctx;
	uint8_t key[16] = { 0 };
	uint8_t iv[16] = { 0 };
	uint8_t ad[20] = "Hi, jochem hier....";
	uint8_t plaintext[84] = "This is a random length string, it is a great test and we are awesome because of it";
	uint8_t res_plain[84] = { 0 };
	uint8_t ciphertext[84] ={ 0 };
	uint8_t tag[16] = { 0 };
	uint8_t res_tag[16] = { 0 };

	key[0] = 0x10;  key[1] = 0x01; iv[0] = 0x10;  iv[2] = 0x02;

	init_AEGIS_ctx_IV(&ctx, key, iv);
	aegisEncrypt(&ctx, ad, 20, plaintext, 84, ciphertext, tag);
	aegisDecrypt(&ctx, ad, 20, ciphertext, 84, res_plain, res_tag);

	assert(equalByteArrays(res_plain, plaintext, 84));

	printf("Test %u - AEGIS passed.\n", (*nb)++);
}

void test_aegis(uint32_t* nb) {
	t1(nb);
	t2(nb);
	t3(nb);
	t4(nb);
	t5(nb);
}