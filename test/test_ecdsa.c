#include "./../include/test.h"

void test_ecdsa_valid(word *nbTest) {
    word privateKey[SIZE], pkx_mont[SIZE], pky_mont[SIZE];
    word r[SIZE], s[SIZE];
    uint8_t message[4] = { 0x85, 0x47, 0xFE, 0xF4 };

    generateKeyPair(privateKey, pkx_mont, pky_mont);
    ecdsaSign(message, 4, privateKey, r, s);
    assert(ecdsaCheck(message, 4, pkx_mont, pky_mont, r, s));
    printf("Test %u - ECDSA valid signature passed.\n", (*nbTest)++);
}

void test_ecdsa_invalid(word *nbTest) {
    word privateKey[SIZE], pkx_mont[SIZE], pky_mont[SIZE];
    word r[SIZE], s[SIZE];
    uint8_t message[4] = { 0x85, 0x47, 0xFE, 0xF4 };

    generateKeyPair(privateKey, pkx_mont, pky_mont);
    ecdsaSign(message, 4, privateKey, r, s);
    s[0] = 0;
    assert(!ecdsaCheck(message, 4, pkx_mont, pky_mont, r, s));
    printf("Test %u - ECDSA invalid signature passed.\n", (*nbTest)++);
}

void test_ecdsa(word *nb) {
    test_ecdsa_valid(&nb);
    test_ecdsa_invalid(&nb);
}