#include "./../include/main_drone.h"

int main_drone(int txPipe, int rxPipe) {
    struct IO_ctx state;
    char buffer[100];
    ssize_t length;

    init_IO_ctx(&state, txPipe, rxPipe);

    /* Receive test message */
    printf("Drone - waiting for incomming stuff\n");
    do {
        length = receive(&state, buffer, 99, 1);
    } while (!state.endOfMessage);
    buffer[length] = NULL;
    
    printf("Drone - %s\n", buffer);

    return 0;
}