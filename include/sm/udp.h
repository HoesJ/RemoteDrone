#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <sys/time.h>
#include <stdint.h>
#include "./../general/params.h"

#ifndef UDP_H_
#define UDP_H_

#define MAX_TRANSFER_LENGTH PIPE_BUFFER_SIZE

int init_socket(int tx_port, int rx_port, int timeout_usec);
int send_message(uint8_t* data, int length);
int receive_message(uint8_t* data);
int close_sockets();

#endif