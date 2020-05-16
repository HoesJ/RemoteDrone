#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <sys/time.h>
#include <stdint.h>
#include <arpa/inet.h>
#include "./../general/params.h"

#ifndef UDP_H_
#define UDP_H_

int init_socket(int tx_port, int rx_port, int timeout_usec);
int flush_buffer();
int send_message(uint8_t* data, int length);
int receive_message(uint8_t* data);
int close_sockets();

#if LIVE_FEED_PORT_IN
int init_live_feed(int port, int receiveOrTransmit, int timeout_usec);
int receive_feed(uint8_t* data);
int send_feed(uint8_t* data, int length);
#endif

#endif