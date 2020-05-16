#include "./../../include/general/print_number.h"

#ifdef SIZE_8
#define FORMAT "0x%02X"
#endif

#ifdef SIZE_16
#define FORMAT "0x%04X"
#endif

#ifdef SIZE_32
#define FORMAT "0x%08X"
#endif

#ifdef SIZE_64
#define FORMAT "0x%016lX"
#endif

/**
 * Print the given number in hexadecimal form.
 */
void printNumber(const word *number, size_t size) {
    size_t i;

    printf("{ ");
    for (i = 0; i < size; i++) {
        if (i != 0)
            printf(" ");
        printf(FORMAT, number[i]);
        if (i != size - 1)
            printf(",");
    }
    printf(" };\n");
}

/**
 * Print the given number in little endian hexadecimal form.
 */
void printBytes(const word *number, size_t size, const char *var) {
    size_t i, j;

    printf("word %s[SIZE];\n", var);
    printf("uint8_t %sBytes[SIZE * sizeof(word)] = { ", var);
    for (i = 0; i < size; i++) {
        if (i != 0)
            printf(" ");
        
        for (j = 0; j < sizeof(word); j++) {
            printf("0x%02X", (uint8_t)(number[i] >> (8 * j)));

            if (j != sizeof(word) - 1)
                printf(", ");
        }

        if (i != size - 1)
                printf(",");
        
        if (i == size / 2 - 1)
            printf("\n\t\t\t\t\t");
    }

    printf(" };\ntoWordArray(%sBytes, %s, SIZE);\n\n", var, var);
}