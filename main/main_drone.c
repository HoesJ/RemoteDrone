#include "./../include/main_drone.h"

static int txPipeGlobal[2];
static int rxPipeGlobal[2];

/**
 * Receive message from the base station. The parameter size gives the
 * number of characters, including NULL termination.
 */
void receive(void *buffer, word size) {
    read(rxPipeGlobal[0], buffer, size);
}

int main_drone(int txPipe[2], int rxPipe[2]) {
    memcpy(txPipeGlobal, txPipe, 2 * sizeof(int));
    memcpy(rxPipeGlobal, rxPipe, 2 * sizeof(int));

    char buffer[100];

    /* Receive test message */
    printf("Drone - waiting for incomming stuff\n");
    read(rxPipe[0], buffer, 21);
    
    printf("Drone - %s\n", buffer);

    return 0;
}