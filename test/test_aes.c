#include "./../include/test.h"

void test_aes(word* nb) {
	uint8_t key[16] = { 0xb6, 0x92, 0xcf, 0x0b, 0x64, 0x3d, 0xbd, 0xf1, 0xbe, 0x9b, 0xc5, 0x00, 0x68, 0x30, 0xb3, 0xfe };
	uint8_t in[16] = { 0x89, 0xd8, 0x10, 0xe8, 0x85, 0x5a, 0xce, 0x68, 0x2d, 0x18, 0x43, 0xd8, 0xcb, 0x12, 0x8f, 0xe4 };
	uint8_t result[16];
	uint8_t out[16] = { 0x49, 0x15, 0x59, 0x8f, 0x55, 0xe5, 0xd7, 0xa0, 0xda, 0xca, 0x94, 0xfa, 0x1f, 0x0a, 0x63, 0xf7 };

	AESRound(result,in,key);

	assert(compareArrays((word*)result, (word*)out, 4));
	printf("Test %u - AES Round passed.\n", (*nb)++);
}