#include "./../include/main_drone.h"

int main_drone(int txPipe, int rxPipe) {
    char buffer[100];

    /* Receive test message */
    printf("Drone - waiting for incomming stuff\n");
    read(rxPipe, buffer, 21);
    
    printf("Drone - %s\n", buffer);

    return 0;
}