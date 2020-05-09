#include "./../general/params.h"

#ifndef UDP_H_
#define UDP_H_

#if WINDOWS
/* See params.h */
/* Notes on including winsock2.h:
	- If you also include windows.h, you have to include winsock2.h before windows.h.
	  windows.h will include the older winsock.h and you will get redefinition errors otherwise
	- For the linker you also have to inlcude extra libraries, namely ws2_32.lib.
	  see:	https://stackoverflow.com/questions/17069802/c-winsock2-errors
			https://stackoverflow.com/questions/4445418/how-to-add-additional-libraries-to-visual-studio-project
			https://stackoverflow.com/questions/17797594/winsock2-h-vs-winsock2-h-and-wsock32-lib-vs-ws2-32-lib
*/
#endif

#if UNIX 
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <sys/time.h>
#include <stdint.h>
#include <arpa/inet.h>
#endif

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