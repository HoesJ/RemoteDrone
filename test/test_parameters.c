#include "./../include/test/test.h"

void test_parameters(uint32_t *nb) {
    assert(MAX_PACKET_SIZE <= 64000);
    assert(NB_CACHED_MESSAGES <= 0xFF);
    printf("Test %u - Parameter ranges passed.\n", (*nb)++);
}