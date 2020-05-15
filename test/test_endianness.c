#include "./../include/test/test.h"

void test_endianness(uint32_t *nb) {
	word array[2] = { 0x00245F47, 0x47EEF893 };
    uint8_t bytes[2 * sizeof(word)];
    word result[2];

    toLittleEndian(array, bytes, 2);
    toWordArray(bytes, result, 2);

    assert(equalWordArrays(array, result, 2));
    printf("Test %u - Conversion to/from little endian passed.\n", (*nb)++);
}