#include "./../include/print_number.h"

/**
 * Print the given number in hexadecimal form.
 */
void printNumber(const word *number, size_t size) {
    size_t i;

    printf("{");
    for (i = 0; i < size; i++) {
        if (i != 0)
            printf(" ");
        printf(" 0x%08X", number[i]);
        if (i != size - 1)
            printf(", ");
    }
    printf(" };\n");
}