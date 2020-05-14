#include "./../include/test/test.h"

void test_parameters(word *nb) {
    assert(MAX_PACKET_SIZE <= 64000);
    printf("Test %u - MAX_PACKET_SIZE <= 64000 passed.\n", (*nb)++);
}