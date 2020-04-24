#include "./../include/main/test.h"
#include "./../include/sm/udp.h"

#define MSGS 5

void udp_test_sender() {
	int i;
	int  recvlen;					// # bytes received
	char buf[MAX_TRANSFER_LENGTH];	// message buffer

	// Ports for direct communication
	// Time-out is set to 5 seconds
	Sleep(2000);
	init_socket(15001, 15000, 5000);

	// Ports for communication over GNU Radio
	// init_socket(21236, 21234, 5);

	// now let's send and receive the messages

	for (i = 0; i < MSGS; i++) {

		sprintf(buf, "Ping message %d to sender\n", i);
		if (send_message(buf, strlen(buf)) == 0) {
			close_sockets();
			printf("S\t- Send failed at message %d\n", i);
			return 0;
		}
		printf("S\t- Sent:     %s\n", buf);


		recvlen = receive_message(buf);
		if (recvlen == -1) {
			printf("S\t- Receive Failed: Timeout\n");
		}
		else {
			buf[recvlen] = 0;
			printf("S\t- Received: %s\n", buf);
		}
	}

	close_sockets();
}

void udp_test_receiver() {
	int i;
	int  recvlen;					// # bytes received
	char buf[MAX_TRANSFER_LENGTH];	// message buffer

	// Ports for direct communication
	// Time-out is set to 5 seconds
	init_socket(15000, 15001, 5000);

	// Ports for communication over GNU Radio
	// init_socket(21235, 21237, 5);

	// now let's send and receive the messages

	for (i = 0; i < MSGS; i++) {
		recvlen = receive_message(buf);
		buf[recvlen] = 0;
		printf("R\t- Received: %s\n", buf);

		sprintf(buf, "Pong message %d from receiver\n", i);
		if (send_message(buf, strlen(buf)) == 0) {
			close_sockets();
			printf("R\t- Send failed at message %d\n", i);
			return 0;
		}
		printf("R\t- Sent:     %s\n", buf);
	}

	close_sockets();
}

void test_udp(word *nb) {
#if UNIX
	/* Create receiver process */
	pidrecv = fork();
	if (pidrecv == -1) {
		printf("Error spawning receiver process\n");
		return void;
	}
	else if (pidrecv == 0) {
		udp_test_receiver();     /* This is the receiver */
	}

	/* Create sender process */
	pidsend = fork();
	if (pidsend == -1) {
		printf("Error spawning sender process\n");
		return void;
	}
	else if (pidsend == 0) {
		udp_test_sender();     /* This is the receiver */
	}

	wait(&pidBS);
	wait(&pidDrone);
#endif
}