#include "./../include/test/test.h"

void test_mod_add(uint32_t *nbTest) {
    word result[SIZE];

	word a1[SIZE];
	uint8_t a1Bytes[SIZE * sizeof(word)] = { 0xA2, 0xF5, 0xCC, 0x15, 0x73, 0x65, 0x7D, 0xF9, 0x78, 0x03, 0xDF, 0x26, 0xE9, 0x97, 0xEE, 0x6A,
					 	  					 0x7C, 0x16, 0xD8, 0x07, 0x57, 0x31, 0xF6, 0x42, 0x4C, 0x11, 0xF7, 0x14, 0x96, 0x55, 0xF3, 0xC0 };
	toWordArray(a1Bytes, a1, SIZE);

	word b1[SIZE];
	uint8_t b1Bytes[SIZE * sizeof(word)] = { 0x4D, 0xFA, 0x25, 0x55, 0xCE, 0xCD, 0x0B, 0xB4, 0xF0, 0xBF, 0x69, 0x52, 0x67, 0xC6, 0xE6, 0x48,
					 	  					 0x42, 0xB1, 0x79, 0xC8, 0x9C, 0x46, 0x0D, 0x8D, 0xB3, 0x91, 0x53, 0xA2, 0xBC, 0x5E, 0xC4, 0xDA };
	toWordArray(b1Bytes, b1, SIZE);

	word n1[SIZE];
	uint8_t n1Bytes[SIZE * sizeof(word)] = { 0xBB, 0x34, 0x44, 0x2E, 0x29, 0x8D, 0x5C, 0xA1, 0x05, 0x89, 0xCE, 0xF4, 0x3B, 0x5A, 0x3D, 0xA8,
					 	  					 0x6E, 0xD4, 0xEE, 0x21, 0xEA, 0x9B, 0x77, 0x6A, 0xFF, 0x2A, 0x6F, 0x5C, 0x6E, 0x1B, 0x0E, 0xDC };
	toWordArray(n1Bytes, n1, SIZE);

	word sol1[SIZE];
	uint8_t sol1Bytes[SIZE * sizeof(word)] = { 0x34, 0xBB, 0xAE, 0x3C, 0x18, 0xA6, 0x2C, 0x0C, 0x64, 0x3A, 0x7A, 0x84, 0x14, 0x04, 0x98, 0x0B,
					        				   0x50, 0xF3, 0x62, 0xAE, 0x09, 0xDC, 0x8B, 0x65, 0x00, 0x78, 0xDB, 0x5A, 0xE4, 0x98, 0xA9, 0xBF };
	toWordArray(sol1Bytes, sol1, SIZE);

	word a2[SIZE];
	uint8_t a2Bytes[SIZE * sizeof(word)] = { 0x7E, 0x9C, 0x05, 0x88, 0xC8, 0x69, 0x3B, 0x18, 0xC7, 0x89, 0x0B, 0x44, 0xE9, 0x91, 0x62, 0x7A,
					 	  					 0x27, 0x94, 0x74, 0x27, 0xF0, 0xFC, 0xF2, 0xB3, 0x9C, 0xC6, 0xE6, 0x4F, 0xBD, 0x4D, 0xB9, 0x97 };
	toWordArray(a2Bytes, a2, SIZE);

	word b2[SIZE];
	uint8_t b2Bytes[SIZE * sizeof(word)] = { 0x5A, 0xDB, 0xAC, 0x11, 0xEC, 0xCF, 0x37, 0xDE, 0xC7, 0x3E, 0xD7, 0xEE, 0x40, 0x70, 0x3C, 0xAF,
					 	  					 0x7E, 0xA8, 0xC5, 0x98, 0x6C, 0x85, 0x29, 0xAD, 0xFE, 0x90, 0xB7, 0x97, 0x1E, 0xC2, 0x78, 0x8D };
	toWordArray(b2Bytes, b2, SIZE);

	word n2[SIZE];
	uint8_t n2Bytes[SIZE * sizeof(word)] = { 0x9D, 0xF6, 0xED, 0xE4, 0x95, 0xED, 0x54, 0x9F, 0x6A, 0x9D, 0x85, 0xF5, 0x34, 0x39, 0xCF, 0xA0,
					 	  					 0x7C, 0xFB, 0x8F, 0x53, 0xD3, 0x9E, 0xD4, 0xDF, 0xAE, 0x06, 0x79, 0xD6, 0x8E, 0xEC, 0x09, 0x9C };
	toWordArray(n2Bytes, n2, SIZE);

	word sol2[SIZE];
	uint8_t sol2Bytes[SIZE * sizeof(word)] =  {0x3B, 0x81, 0xC4, 0xB4, 0x1E, 0x4C, 0x1E, 0x57, 0x24, 0x2B, 0x5D, 0x3D, 0xF5, 0xC8, 0xCF, 0x88,
					   						   0x29, 0x41, 0xAA, 0x6C, 0x89, 0xE3, 0x47, 0x81, 0xEC, 0x50, 0x25, 0x11, 0x4D, 0x23, 0x28, 0x89 };
	toWordArray(sol2Bytes, sol2, SIZE);
	
	

    mod_add(a1, b1, n1, result);
    assert(equalWordArrays(sol1, result, SIZE));
    printf("Test %u - Modular addition passed.\n", (*nbTest)++);

    mod_add(a2, b2, n2, result);
    assert(equalWordArrays(sol2, result, SIZE));
    printf("Test %u - Modular addition passed.\n", (*nbTest)++);
}

void test_mod_sub(uint32_t *nbTest) {
    word result[SIZE];

	word a1[SIZE];
	uint8_t a1Bytes[SIZE * sizeof(word)] = { 0xAD, 0x75, 0x02, 0x88, 0xE0, 0x2A, 0x5E, 0xD9, 0x2C, 0xF7, 0x7E, 0x4C, 0x58, 0x31, 0x6B, 0x2A,
											 0x97, 0xB8, 0x7F, 0x90, 0xDD, 0x19, 0x90, 0x5E, 0x3C, 0xCA, 0x9E, 0xC5, 0x65, 0xC6, 0x83, 0x87 };
	toWordArray(a1Bytes, a1, SIZE);

	word b1[SIZE];
	uint8_t b1Bytes[SIZE * sizeof(word)] = { 0x60, 0xE5, 0x11, 0x35, 0x4C, 0xD2, 0xC8, 0xB6, 0xAF, 0x8C, 0xF8, 0xD7, 0xA2, 0x59, 0x69, 0x82,
											 0x65, 0xF8, 0x56, 0x2E, 0x38, 0xDB, 0x44, 0xFB, 0x20, 0x9F, 0xE8, 0xE5, 0x14, 0x31, 0x7A, 0x8C };
	toWordArray(b1Bytes, b1, SIZE);

	word n1[SIZE];
	uint8_t n1Bytes[SIZE * sizeof(word)] = { 0x01, 0x61, 0x55, 0x36, 0xAE, 0xE1, 0xFF, 0x8E, 0xD8, 0xFF, 0xE3, 0x3D, 0x96, 0x8E, 0xEB, 0xDF,
											 0xAA, 0x12, 0x98, 0x1D, 0x3B, 0xBA, 0x45, 0x94, 0xC3, 0xD2, 0x81, 0x97, 0x14, 0xA5, 0x4C, 0x90 };
	toWordArray(n1Bytes, n1, SIZE);

	word sol1[SIZE];
	uint8_t sol1Bytes[SIZE * sizeof(word)] = { 0x4E, 0xF1, 0x45, 0x89, 0x42, 0x3A, 0x95, 0xB1, 0x55, 0x6A, 0x6A, 0xB2, 0x4B, 0x66, 0xED, 0x87,
											   0xDC, 0xD2, 0xC0, 0x7F, 0xE0, 0xF8, 0x90, 0xF7, 0xDE, 0xFD, 0x37, 0x77, 0x65, 0x3A, 0x56, 0x8B };
	toWordArray(sol1Bytes, sol1, SIZE);

	word a2[SIZE];
	uint8_t a2Bytes[SIZE * sizeof(word)] = { 0xEE, 0xE0, 0xEA, 0xC8, 0x87, 0xEF, 0xB7, 0xBD, 0x53, 0xEE, 0x97, 0x4A, 0x03, 0x95, 0x21, 0x4E,
											 0xA5, 0x87, 0xFE, 0xB7, 0xC2, 0x4A, 0xBF, 0xFD, 0x88, 0x0A, 0xDB, 0xBC, 0xCE, 0xD0, 0xB5, 0xBE };
	toWordArray(a2Bytes, a2, SIZE);

	word b2[SIZE];
	uint8_t b2Bytes[SIZE * sizeof(word)] = { 0xDF, 0xA0, 0x71, 0xEA, 0xBC, 0xCC, 0x77, 0xB3, 0xD3, 0x64, 0x14, 0x52, 0x9B, 0x6F, 0xB8, 0xCF,
										     0x65, 0x3F, 0x2D, 0xD1, 0x5E, 0xCE, 0xDE, 0x88, 0x47, 0x7D, 0x79, 0xC3, 0x93, 0x7E, 0x42, 0xB0 };
	toWordArray(b2Bytes, b2, SIZE);

	word n2[SIZE];
	uint8_t n2Bytes[SIZE * sizeof(word)] = { 0xDD, 0x3D, 0xAF, 0xA7, 0x34, 0x04, 0x4A, 0x34, 0x91, 0x47, 0x89, 0x5A, 0x04, 0x92, 0x1E, 0x96,
											 0x65, 0x0D, 0x17, 0xCF, 0xBB, 0xF0, 0xEC, 0x1C, 0x2F, 0x7C, 0x71, 0x4F, 0xE2, 0xE6, 0x17, 0xD9 };
	toWordArray(n2Bytes, n2, SIZE);

	word sol2[SIZE];
	uint8_t sol2Bytes[SIZE * sizeof(word)] = { 0x0F, 0x40, 0x79, 0xDE, 0xCA, 0x22, 0x40, 0x0A, 0x80, 0x89, 0x83, 0xF8, 0x67, 0x25, 0x69, 0x7E,
											   0x3F, 0x48, 0xD1, 0xE6, 0x63, 0x7C, 0xE0, 0x74, 0x41, 0x8D, 0x61, 0xF9, 0x3A, 0x52, 0x73, 0x0E };
	toWordArray(sol2Bytes, sol2, SIZE);



    mod_sub(a1, b1, n1, result);
    assert(equalWordArrays(sol1, result, SIZE));
    printf("Test %u - Modular subtraction passed.\n", (*nbTest)++);

    mod_sub(a2, b2, n2, result);
    assert(equalWordArrays(sol2, result, SIZE));
    printf("Test %u - Modular subtraction passed.\n", (*nbTest)++);
}

void test_montgomery(uint32_t *nbTest) {
    word result[SIZE];

	word a1[SIZE];
	uint8_t a1Bytes[SIZE * sizeof(word)] = { 0x2A, 0x24, 0x2D, 0xB6, 0x1C, 0xFA, 0x4A, 0xDD, 0x38, 0x82, 0xB7, 0x9F, 0x26, 0x29, 0x82, 0xA2,
											 0x97, 0xB8, 0x63, 0x71, 0x15, 0xAC, 0x78, 0xD5, 0x89, 0xAD, 0xF3, 0xD9, 0x5A, 0x71, 0x18, 0x88 };
	toWordArray(a1Bytes, a1, SIZE);

	word b1[SIZE];
	uint8_t b1Bytes[SIZE * sizeof(word)] = { 0x08, 0x58, 0x8E, 0x97, 0x82, 0x97, 0x42, 0x99, 0x16, 0x92, 0x9F, 0xFE, 0x1B, 0xDF, 0xD3, 0x34,
											 0xD6, 0x37, 0x28, 0x2C, 0x28, 0x86, 0x44, 0xAB, 0x1F, 0xFB, 0xEE, 0x08, 0x5A, 0x71, 0x04, 0x87 };
	toWordArray(b1Bytes, b1, SIZE);

	word n1[SIZE];
	uint8_t n1Bytes[SIZE * sizeof(word)] = { 0xD1, 0xDE, 0x73, 0x5B, 0xC6, 0x74, 0x57, 0x5D, 0x9B, 0x3E, 0xFF, 0x0D, 0x80, 0xF0, 0xA9, 0x57,
											 0x00, 0x6E, 0x77, 0x0B, 0xC9, 0x11, 0xDE, 0x63, 0x9A, 0x45, 0xC9, 0xF8, 0x51, 0x43, 0x09, 0x8D };
	toWordArray(n1Bytes, n1, SIZE);

	word n_prime1[SIZE];
	uint8_t n_prime1Bytes[SIZE * sizeof(word)] = { 0xCF, 0xC5, 0x58, 0x5B, 0xC3, 0xB6, 0xDF, 0xA1, 0x0C, 0x12, 0x9F, 0x52, 0x8C, 0x6E, 0x33, 0x00,
												   0x69, 0xC0, 0x6E, 0x78, 0xA7, 0x5B, 0xC8, 0xD5, 0x6F, 0x56, 0x43, 0xDA, 0x37, 0x26, 0x17, 0xA9 };
	toWordArray(n_prime1Bytes, n_prime1, SIZE);

	word sol1[SIZE];
	uint8_t sol1Bytes[SIZE * sizeof(word)] = { 0xC8, 0xAE, 0x8C, 0x0C, 0xDF, 0x75, 0x60, 0x82, 0xC2, 0x19, 0x88, 0x02, 0x6B, 0xEC, 0x29, 0x92,
											   0x81, 0x8F, 0x5B, 0x58, 0xB2, 0x17, 0xA1, 0xB7, 0xFF, 0xC8, 0x83, 0xCE, 0xE0, 0xDB, 0x07, 0x1C };
	toWordArray(sol1Bytes, sol1, SIZE);

	word a2[SIZE];
	uint8_t a2Bytes[SIZE * sizeof(word)] = { 0x18, 0x39, 0x9F, 0xB6, 0x54, 0xE4, 0xF7, 0xCE, 0xA8, 0x07, 0x92, 0xB0, 0x8A, 0x81, 0x00, 0x77,
											 0xC1, 0x22, 0x18, 0x5B, 0x75, 0x33, 0x66, 0x92, 0xD9, 0xD0, 0xD2, 0xB7, 0xA3, 0x92, 0x61, 0x8B };
	toWordArray(a2Bytes, a2, SIZE);

	word b2[SIZE];
	uint8_t b2Bytes[SIZE * sizeof(word)] = { 0x04, 0xDC, 0xDA, 0x73, 0x1F, 0x70, 0x9B, 0x8C, 0x7C, 0x68, 0xE6, 0x89, 0xDC, 0x47, 0x55, 0xE2,
											 0xC1, 0xB1, 0x87, 0x47, 0xA8, 0x22, 0x61, 0x12, 0x2B, 0x8F, 0xF2, 0x07, 0x38, 0x24, 0xD3, 0x8B };
	toWordArray(b2Bytes, b2, SIZE);

	word n2[SIZE];
	uint8_t n2Bytes[SIZE * sizeof(word)] = { 0x1B, 0x18, 0x1E, 0x7B, 0xF6, 0x0C, 0xCC, 0xBC, 0xB1, 0x3E, 0xBF, 0x6F, 0xEB, 0x91, 0xEC, 0x3A,
											 0xC3, 0x32, 0x69, 0xB7, 0x36, 0x0A, 0x86, 0x5A, 0x5F, 0x7A, 0x46, 0xBD, 0x46, 0xCA, 0xC3, 0x90 };
	toWordArray(n2Bytes, n2, SIZE);

	word n_prime2[SIZE];
	uint8_t n_prime2Bytes[SIZE * sizeof(word)] = { 0xED, 0xFD, 0x03, 0xED, 0xE8, 0xE6, 0x34, 0x6F, 0x97, 0x95, 0x29, 0x62, 0xC2, 0xBD, 0x1B, 0xE8,
												   0xCA, 0x4D, 0x98, 0x2F, 0x55, 0x02, 0x57, 0xD2, 0x39, 0xAE, 0x04, 0x73, 0x15, 0x8A, 0x6A, 0xD5 };
	toWordArray(n_prime2Bytes, n_prime2, SIZE);

	word sol2[SIZE];
	uint8_t sol2Bytes[SIZE * sizeof(word)] = { 0x0C, 0x72, 0x33, 0xDD, 0x3B, 0xA6, 0xA0, 0xDF, 0x86, 0x37, 0x19, 0x32, 0x1A, 0x60, 0x32, 0x3B,
											   0x8C, 0xDE, 0xD6, 0x3C, 0xF4, 0x10, 0x4F, 0x0F, 0x58, 0xB9, 0x23, 0xD8, 0xD0, 0x6C, 0x08, 0x6C };
	toWordArray(sol2Bytes, sol2, SIZE);



    montMul(a1, b1, n1, n_prime1, result);
    assert(equalWordArrays(sol1, result, SIZE));

	montMul(a2, b2, n2, n_prime2, result);
	assert(equalWordArrays(sol2, result, SIZE));

    printf("Test %u - Montgomery multiplication passed.\n", (*nbTest)++);
}

void test_mod_inv(uint32_t *nbTest) {
    word res[SIZE];

	word x1[SIZE];
	uint8_t x1Bytes[SIZE * sizeof(word)] = { 0x5F, 0x82, 0x7D, 0x8A, 0x21, 0xFB, 0xF5, 0x84, 0x2B, 0xF0, 0x4A, 0xB2, 0x1F, 0x51, 0x17, 0x36,
											 0x4D, 0x67, 0x92, 0x0E, 0x94, 0xA2, 0x7D, 0x00, 0x27, 0x8A, 0x2F, 0xEA, 0x4A, 0xB5, 0xAD, 0xB9 };
	toWordArray(x1Bytes, x1, SIZE);

	word p1[SIZE];
	uint8_t p1Bytes[SIZE * sizeof(word)] = { 0x3B, 0x0B, 0x59, 0x31, 0xEC, 0xB4, 0xAB, 0x1B, 0x6D, 0xF1, 0x14, 0xB0, 0x4E, 0x8A, 0x4B, 0x22,
											 0x61, 0x23, 0x89, 0x6C, 0x44, 0x16, 0xD0, 0xFA, 0x5E, 0x7F, 0x32, 0xC0, 0x0A, 0x82, 0x03, 0xE0 };
	toWordArray(p1Bytes, p1, SIZE);

	word sol1[SIZE];
	uint8_t sol1Bytes[SIZE * sizeof(word)] = { 0x54, 0x09, 0x7C, 0x33, 0x14, 0x42, 0x4E, 0x54, 0x9F, 0x26, 0xFA, 0x37, 0x20, 0x5A, 0x60, 0x8F,
											   0xC8, 0x97, 0x13, 0xCA, 0x9C, 0xAC, 0x7C, 0xB9, 0xE4, 0x9B, 0x9B, 0x4A, 0x7E, 0xB1, 0xC8, 0x55 };
	toWordArray(sol1Bytes, sol1, SIZE);

	word x2[SIZE];
	uint8_t x2Bytes[SIZE * sizeof(word)] = { 0x1A, 0xCA, 0x53, 0x6E, 0x0D, 0x6B, 0x99, 0x87, 0x5E, 0xE5, 0xD7, 0x60, 0x46, 0xEF, 0xFA, 0x74,
											 0x27, 0x0A, 0x9C, 0x02, 0x8C, 0x73, 0x32, 0x78, 0x9E, 0x07, 0xE4, 0x69, 0x23, 0xF0, 0x1F, 0x8D };
	toWordArray(x2Bytes, x2, SIZE);

	word p2[SIZE];
	uint8_t p2Bytes[SIZE * sizeof(word)] = { 0x53, 0x84, 0x9A, 0xB1, 0x6B, 0x05, 0x82, 0xBE, 0xA3, 0x49, 0xEF, 0x25, 0x19, 0xF2, 0x78, 0x5F,
											 0xAA, 0xC8, 0xC9, 0x1A, 0xEE, 0x3C, 0x94, 0x16, 0xC1, 0x5A, 0x7C, 0x9A, 0x2C, 0xE6, 0xAC, 0x9A };
	toWordArray(p2Bytes, p2, SIZE);

	word sol2[SIZE];
	uint8_t sol2Bytes[SIZE * sizeof(word)] = { 0xAD, 0x33, 0x3E, 0xBB, 0x60, 0x5F, 0x76, 0xD0, 0xB3, 0x76, 0x3A, 0xDB, 0xF3, 0x4C, 0x74, 0x2C,
										       0xE3, 0xDF, 0x7B, 0xAE, 0xE4, 0x91, 0x30, 0x60, 0xEE, 0xD4, 0x9A, 0xF1, 0x84, 0x26, 0xC5, 0x5F };
	toWordArray(sol2Bytes, sol2, SIZE);

	word x3[SIZE];
	uint8_t x3Bytes[SIZE * sizeof(word)] = { 0x79, 0x73, 0x8B, 0xF7, 0x4F, 0x29, 0x96, 0x0C, 0x66, 0xC7, 0xBD, 0x20, 0x3F, 0xB0, 0x99, 0x57,
											 0x5B, 0xEA, 0xDE, 0x4D, 0x35, 0x40, 0xDD, 0x0E, 0xBF, 0x4B, 0x42, 0x84, 0x1A, 0x5C, 0x9E, 0x81 };
	toWordArray(x3Bytes, x3, SIZE);

	word p3[SIZE];
	uint8_t p3Bytes[SIZE * sizeof(word)] = { 0x1D, 0x94, 0xD3, 0x2E, 0x68, 0xCF, 0xA3, 0xC6, 0x85, 0xB3, 0x3C, 0x2B, 0x04, 0x06, 0x51, 0x21,
											 0x7F, 0x3B, 0x5A, 0x10, 0x8B, 0x64, 0x8B, 0x89, 0xD2, 0x00, 0x22, 0xE4, 0x48, 0x8A, 0x62, 0x84 };
	toWordArray(p3Bytes, p3, SIZE);

	word sol3[SIZE];
	uint8_t sol3Bytes[SIZE * sizeof(word)] = { 0xB0, 0x3D, 0xDE, 0xD5, 0xF9, 0xB5, 0xF6, 0x3E, 0x5D, 0x95, 0xAB, 0x2D, 0xC3, 0xF5, 0x29, 0xAE,
											   0x34, 0x47, 0x0D, 0x04, 0xEF, 0x2F, 0x73, 0xD1, 0xBA, 0xB1, 0x58, 0x9D, 0xE1, 0xD5, 0x4C, 0x83 };
	toWordArray(sol3Bytes, sol3, SIZE);



    mod_inv(x1, p1, res);
    assert(equalWordArrays(sol1, res, SIZE));
    mod_inv(x2, p2, res);
    assert(equalWordArrays(sol2, res, SIZE));
    mod_inv(x3, p3, res);
    assert(equalWordArrays(sol3, res, SIZE));

    printf("Test %u - Modular inversion passed.\n", (*nbTest)++);
}

void test_mod_arithmetic(uint32_t *nbTest) {
    test_mod_add(nbTest);
    test_mod_sub(nbTest);
    test_montgomery(nbTest);
    test_mod_inv(nbTest);
}