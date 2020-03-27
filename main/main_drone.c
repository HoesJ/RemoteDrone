#include "./../include/main_drone.h"

int main_drone(int txPipe, int rxPipe) {
	return 0;
}

#if WINDOWS
int main_drone_win(struct threadParam* param) {
	struct IO_ctx state;
	char buffer[100];
	ssize_t length;

	init_IO_ctx(&state, (int)param->txPipe, (int)param->rxPipe);

	/* Receive test message */
	printf("Drone -\twaiting for incomming stuff\n");
	do {
		length = receive(&state, buffer, 99, 1);
	} while (!state.endOfMessage);
	buffer[length] = NULL;

	printf("Drone -\t%s\n", buffer);

	/* Receive test message */
	printf("Drone -\twaiting for incomming stuff\n");
	do {
		length = receive(&state, buffer, 99, 1);
	} while (!state.endOfMessage);
	buffer[length] = NULL;

	printf("Drone -\t%s\n", buffer);

	return 0;
}
#endif