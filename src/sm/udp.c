#include "./../../include/sm/udp.h"

/* IP address of destination */
const char DEST_IP[256] = "127.0.0.1";

/* Hold data to be sent. */
uint8_t buf[MAX_PACKET_SIZE];
size_t buf_index = 0;

struct sockaddr_in rx_addr;
struct sockaddr_in tx_addr;
int fd_tx;
int fd_rx;

int inet_aton(const char *cp, struct in_addr *addr);

int init_socket(int tx_port, int rx_port, int timeout_usec) {
	int flags;
	struct timeval read_timeout;

	/* create tx and rx sockets */
	if ((fd_tx = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		printf("Cannot create TX socket\n");
		return 0;
	}

	if ((fd_rx = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		printf("Cannot create RX socket\n");
		return 0;
	}

	if (timeout_usec == 0) {
		/* make socket non-blocking */
		flags = fcntl(fd_rx, F_GETFL, 0);
  		fcntl(fd_rx, F_SETFL, flags | O_NONBLOCK);
	} else {
		/* set timeout for receiver */
		read_timeout.tv_sec = 0;
		read_timeout.tv_usec = timeout_usec;

		setsockopt(fd_rx, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof(read_timeout));
	}

	/* bind rx socket to all valid addresses, and a specific port */
	memset((char *)&rx_addr, 0, sizeof(rx_addr));
	rx_addr.sin_family = AF_INET;
	rx_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	rx_addr.sin_port = htons(rx_port);

	if (bind(fd_rx, (struct sockaddr *)&rx_addr, sizeof(rx_addr)) < 0) {
		printf("Bind failed\n");
		return 0;
	}

	/* now define tx_addr, the address to whom we want to send messages */
	memset((char *)&tx_addr, 0, sizeof(tx_addr));
	tx_addr.sin_family = AF_INET;
	tx_addr.sin_port = htons(tx_port);

	if (inet_aton(DEST_IP, &tx_addr.sin_addr) == 0) {
		printf("inet_aton() failed\n");
		return 0;
	}

	return 1;
}

int flush_buffer() {
	size_t tmp;

	if (buf_index == 0)
		return 0;

	tmp = buf_index;
	buf_index = 0;
	if (sendto(
		fd_tx,
		buf,
		tmp,
		0,
		(struct sockaddr *)&tx_addr,
		sizeof(tx_addr)) == -1)
		return -1;
	return 0;
}

int send_message(uint8_t* data, int length) {
	while (length > 0) {
		if (buf_index + length > MAX_PACKET_SIZE) {
			memcpy(buf + buf_index, data, MAX_PACKET_SIZE - buf_index);
			data += (MAX_PACKET_SIZE - buf_index);
			length -= (MAX_PACKET_SIZE - buf_index);
			buf_index = MAX_PACKET_SIZE;
			if (flush_buffer() == -1)
				return -1;
		}
		else {
			memcpy(buf + buf_index, data, length);
			buf_index += length;
			length = 0;
		}
	}

	return 0;
}

int receive_message(uint8_t* data) {
	socklen_t addrlen = sizeof(rx_addr);

	return recvfrom(
		fd_rx,
		data,
		MAX_PACKET_SIZE,
		0,
		(struct sockaddr *)&rx_addr,
		&addrlen);
}

#if LIVE_FEED_PORT_IN
int live_feed;
struct sockaddr_in live_feed_addr;

int init_live_feed(int port, int receiveOrTransmit, int timeout_usec) {
	int flags;
	struct timeval read_timeout;

	/* Create socket */
	if ((live_feed = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		printf("Cannot create live feed socket\n");
		close_sockets();
		return 0;
	}

	/* Set timeout of live feed socket */
	if (timeout_usec == 0) {
		/* make socket non-blocking */
		flags = fcntl(live_feed, F_GETFL, 0);
		fcntl(live_feed, F_SETFL, flags | O_NONBLOCK);
	}
	else {
		/* set timeout for receiver */
		read_timeout.tv_sec = 0;
		read_timeout.tv_usec = timeout_usec;
		setsockopt(live_feed, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof(read_timeout));
	}

	if (receiveOrTransmit == 0) { /* Then listen */
		/* Bind live feed socket to all valid addresses and a specific port */
		memset((uint8_t*)&live_feed_addr, 0, sizeof(live_feed_addr));
		live_feed_addr.sin_family = AF_INET;
		live_feed_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		live_feed_addr.sin_port = htons(port);

		printf("Life feed bind on %d.\n", port);
		if (bind(live_feed, (struct sockaddr *)&live_feed_addr, sizeof(live_feed_addr)) < 0) {
			printf("Life feed bind failed:\n");
			close_sockets();
			return 0;
		}
		printf("Life feed bind success.\n");
	}
	else { /* Then forward */
		memset((char *)&live_feed_addr, 0, sizeof(live_feed_addr));
		live_feed_addr.sin_family = AF_INET;
		live_feed_addr.sin_port = htons(port);

		if (inet_aton("127.0.0.1", &live_feed_addr.sin_addr) == 0) {
			printf("inet_aton() failed\n");
			return 0;
		}
	}

	return 1;
}

int receive_feed(uint8_t* data) {
	int live_feed_addr_len = sizeof(live_feed_addr);
	return recvfrom(live_feed, data, MP4_UDP_SIZE, 0, (struct sockaddr *)&live_feed_addr, (socklen_t*)&live_feed_addr_len);
}

int send_feed(uint8_t* data, int length) {
	if (sendto(live_feed, data, length, 0, (struct sockaddr *)&live_feed_addr, sizeof(live_feed_addr)) == -1)
		return -1;
	return 0;
}
#endif

int close_sockets() {
	close(fd_tx);
	close(fd_rx);
#if LIVE_FEED_PORT_IN
	close(live_feed);
#endif
	
	return 0;
}