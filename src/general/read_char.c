#include "./../../include/general/read_char.h"

static uint8_t input = 0;
static uint8_t nbTimesRead = 0;

uint8_t readChar() {
    char ch;

    /* Read character from input */
    if (input != 0) {
        ch = input;
        input = 0;
        return ch;
    } else
        return getchar();
}

void writeChar(uint8_t ch) {
    /* If same input is read too many times, we ignore it */
    if (++nbTimesRead % 16 == 0)
        input = 0;
    else
        input = ch;
}