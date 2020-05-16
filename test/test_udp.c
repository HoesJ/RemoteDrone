#include "./../include/test/test.h"

void udp_test_sender() {
	uint8_t buf = 25;					/* message buffer */

	/* Ports for direct communication */
	/* Time-out is set to 5 seconds */
	init_socket(15001, 15000, 500000);

	send_message(&buf, 1);
	flush_buffer();
	close_sockets();
}

void udp_test_receiver() {
	uint8_t buf[MAX_PACKET_SIZE];		/* message buffer */

	/* Ports for direct communication */
	/* Time-out is set to 5 seconds */
	init_socket(15000, 15001, 500000);

	receive_message(buf);
	assert(buf[0] == 25);

	close_sockets();
}

void test_udp(word *nb) {
#if UNIX
	pid_t pidrecv, pidsend;
	/* Create receiver process */
	pidrecv = fork();
	if (pidrecv == 0) {
		udp_test_receiver();    /* This is the receiver */
		exit(0);
	}

	/* Create sender process */
	pidsend = fork();
	if (pidsend == 0) {
		udp_test_sender();     	/* This is the sender */
		exit(0);
	}

	wait(&pidrecv);
	wait(&pidsend);
	printf("Test %u - UDP socket send/receive message passed.\n", (*nb)++);
#endif
}