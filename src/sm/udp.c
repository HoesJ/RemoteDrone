#include "./../../include/sm/udp.h"

#if UNIX
struct sockaddr_in rx_addr; 
struct sockaddr_in tx_addr; 
int fd_tx;
int fd_rx;

/* Hold data to be sent. */
uint8_t buf[MAX_PACKET_SIZE];
size_t buf_index = 0;

int init_socket(int tx_port, int rx_port, int timeout_usec) {
	struct timeval read_timeout;
	
	/* create tx and rx sockets */
	if ((fd_tx=socket(AF_INET, SOCK_DGRAM, 0))==-1) {
		printf("Cannot create TX socket\n");
        return 0;
    }

    if ((fd_rx=socket(AF_INET, SOCK_DGRAM, 0))==-1) {
		printf("Cannot create RX socket\n");
        return 0;
    }

    /* Set timeout for receiver */
    read_timeout.tv_sec = 0;
    read_timeout.tv_usec = timeout_usec;

    setsockopt(fd_rx, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof(read_timeout));
    
    /* bind rx socket to all valid addresses, and a specific port */
	memset((char *)&rx_addr, 0, sizeof(rx_addr));
	rx_addr.sin_family = AF_INET;
	rx_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    rx_addr.sin_port = htons(rx_port);

	if (bind(fd_rx, (struct sockaddr *)&rx_addr, sizeof(rx_addr)) < 0) {
		printf("bind failed");
		return 0;
	}

    /* now define tx_addr, the address to whom we want to send messages */
    memset((char *) &tx_addr, 0, sizeof(tx_addr));
	tx_addr.sin_family = AF_INET;
	tx_addr.sin_port = htons(tx_port);

	if (inet_aton(DEST_IP, &tx_addr.sin_addr)==0) {
		printf("inet_aton() failed\n");
		return 0;
	}

    return 1;
}

int close_sockets() {
    close(fd_tx);
    close(fd_rx);

	return 0;
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
            sizeof(tx_addr))==-1)
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
		} else {
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
        MAX_TRANSFER_LENGTH, 
        0, 
        (struct sockaddr *)&rx_addr, 
        &addrlen);
}
#endif

#if WINDOWS
struct sockaddr_in rx_addr;
struct sockaddr_in tx_addr;
struct timeval read_timeout;
SOCKET tx;
SOCKET rx;

int close_sockets() {
	closesocket(tx);
	closesocket(rx);
	WSACleanup();
}

int init_socket(int tx_port, int rx_port, int timeout_sec) {
	WSADATA wsaData;

	/* Initialise winsock */
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("WSAStartup failed.\n");
		return 0;
	}
	printf("Initialised.\n");

	/* Create Tx and Rx sockets */
	if ((tx = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
		printf("Cannot create TX socket\n");
		close_sockets();
		return 0;
	}

	if ((rx = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
		printf("Cannot create RX socket\n");
		close_sockets();
		return 0;
	}

	printf("Sockets created.\n");

	/* Set timeout of rx socket */
	read_timeout.tv_sec = timeout_sec;
	read_timeout.tv_usec = 0;

	setsockopt(rx, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof(read_timeout));

	/* Bind rx socket to all valid addresses and a specific port */
	memset((uint8_t*)&rx_addr, 0, sizeof(rx_addr));
	rx_addr.sin_family = AF_INET;
	rx_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	rx_addr.sin_port = htons(rx_port);

	printf("RX bind on %d.\n", rx_port);
	if (bind(rx, (SOCKADDR *)&rx_addr, sizeof(rx_addr)) == SOCKET_ERROR) {
		printf("RX bind failed: %ld.\n", WSAGetLastError());
		close_sockets();
		return 0;
	}
	printf("RX bind success.\n");

	/* Set parameters for Tx socket */
	memset((char *)&tx_addr, 0, sizeof(tx_addr));
	tx_addr.sin_family = AF_INET;
	tx_addr.sin_port = htons(tx_port);
	tx_addr.sin_addr.s_addr = inet_addr(DEST_IP);

	if (tx_addr.sin_addr.s_addr == INADDR_NONE || tx_addr.sin_addr.s_addr == INADDR_ANY) {
		printf("Invalid destination IP\n");
		close_sockets();
		return 0;
	}

	return 1;
}

int send_message(uint8_t* data, int length) {
	if (sendto(rx, data, length, 0, (SOCKADDR*)&tx_addr, sizeof(tx_addr)) == SOCKET_ERROR)
		return 0;
	return 1;
}

int receive_message(uint8_t* data) {
	int rx_addr_len = sizeof(rx_addr);
	return recvfrom(rx, data, MAX_TRANSFER_LENGTH, 0, (SOCKADDR*)&rx_addr, &rx_addr_len);
}
#endif