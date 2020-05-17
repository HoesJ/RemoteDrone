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

/* Test has been deactivated */
void test_udp(uint32_t *nb) {

}