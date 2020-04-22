#include "./../../include/general/read_char.h"

uint8_t readChar() {
    return
    #if UNIX
        getchar();
    #else
        getch();
    #endif
}