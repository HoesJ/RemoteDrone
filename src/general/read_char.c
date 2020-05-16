#include "./../../include/general/read_char.h"

static uint8_t input = 0;
static uint8_t initDone = 0;

void initNonBlockingIO() {
    struct termios new_settings;

    /* Only run this function once */
    if (initDone)
        return;
    
    tcgetattr(0, &new_settings);

    new_settings.c_lflag &= ~ICANON;
    new_settings.c_lflag &= ~ECHO;
    new_settings.c_lflag &= ~ISIG;
    new_settings.c_cc[VMIN] = 0;
    new_settings.c_cc[VTIME] = 0;

    tcsetattr(0, TCSANOW, &new_settings);
    initDone = 1;
}

uint8_t readChar() {
    char ch;

    initNonBlockingIO();

    /* Read character from input */
    if (input != 0) {
        ch = input;
        input = 0;
        return ch;
    } else
        return getchar();
}

writeChar(uint8_t ch) {
    input = ch;
}