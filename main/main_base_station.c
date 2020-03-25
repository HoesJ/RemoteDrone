#include "./../include/main_base_station.h"

int main_base_station(int txPipe[2], int rxPipe[2]) {
    char testmsg[] = "Hallo drone, BS here";

    /* Send test to drone */
    write(txPipe[1], testmsg, 21);

    printf("BS - Waiting for response");
    read(rxPipe[0], testmsg, 4);

    return 0;
}