#include "./../include/main_drone.h"

int main_drone(int txPipe[2], int rxPipe[2]) {
    memcpy(txPipe, txPipe, 2 * sizeof(int));
    memcpy(rxPipe, rxPipe, 2 * sizeof(int));

    char buffer[100];

    /* Receive test message */
    printf("Drone - waiting for incomming stuff\n");
    read(rxPipe[0], buffer, 21);
    
    printf("Drone - %s\n", buffer);

    return 0;
}