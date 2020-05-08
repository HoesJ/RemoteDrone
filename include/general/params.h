#include "platform.h"
#include "variable_params.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>

#if UNIX
#include <sys/wait.h>
#include <unistd.h>
#include <ncurses.h>
#include "kbhit.h"
#endif

#if WINDOWS
#include <winsock2.h>
#include <Windows.h>
#include <process.h>
#include <conio.h> 					/* For keyboard control */
#define sleep
#endif

#ifndef PARAMS_H_
#define PARAMS_H_

/* Stuff for simulation with different processes and pipes */
#if WINDOWS
typedef int ssize_t;

struct threadParam {
	struct pipe* txPipe;
	struct pipe* rxPipe;
};

struct pipe {
	uint8_t buffer[2048];
	int		readOffset;
	int		writeOffset;
};
#endif

/* Type definitions */
typedef int32_t   signed_word;
typedef uint32_t  word;
typedef uint64_t  double_word;
typedef double    float_word;
typedef size_t  (*checkInput)(uint8_t *buffer, size_t size);
typedef void	(*writeOutput)(uint8_t *buffer, size_t size);

/* Overall important constants */
#define BITS 32
#define SIZE 8

/* AEGIS constants */
#define AEGIS_KEY_NB		16
#define AEGIS_IV_NB			16
#define AEGIS_MAC_NB		16

/* Decoding - Encoding */
#define FIELD_TYPE_NB		1
#define FIELD_LENGTH_NB		3
#define FIELD_IV_NB			AEGIS_IV_NB
#define	FIELD_TARGET_NB		8
#define FIELD_SEQNB_NB		4
#define FIELD_CURVEPOINT_NB	64
#define FIELD_SIGN_NB		64
#define FIELD_HEADER_NB		(FIELD_TYPE_NB + FIELD_LENGTH_NB + FIELD_IV_NB + FIELD_TARGET_NB + FIELD_SEQNB_NB)

/* Reliable/unreliable video feed */
#define RELIABLE_FEED   0
#define PACKET_INTERVAL	1500			/* In microseconds */
#define FEED_DEBUG		0

/* Variables for socket communication. */
#define TIMEOUT_SOC_UNIX 0			/* In microseconds */
#define TIMEOUT_SOC_WIN	 0      	/* More magic */
#define BS_PORT 		 9999
#define DRONE_PORT 		 9998
#define LIVE_FEED_PORT   9997

/* Communication constants */
#define PIPE_BUFFER_SIZE	65535														/* Max size of UDP packet */
#define DECODER_BUFFER_SIZE 1500														/* Should be large enough for video packet */
#define MAX_MESSAGE_NB		(FIELD_HEADER_NB + DECODER_BUFFER_SIZE + AEGIS_MAC_NB)		/* Total max message size, including header */
#if (11 * MAX_MESSAGE_NB / 10) < 64000
#define MAX_PACKET_SIZE		(11 * MAX_MESSAGE_NB / 10)									/* Extra space for byte stuffing */
#else
#define MAX_PACKET_SIZE		64000
#endif
#define MAKE_BER			0
#define FRAC_BER			0.000005

/* Size of transmitted messages */
#define KEP1_MESSAGE_BYTES  		80
#define KEP2_MESSAGE_BYTES  		176
#define KEP3_MESSAGE_BYTES  		112
#define KEP4_MESSAGE_BYTES  		52
#define MIN_MESSAGE_BYTES  			48
#define SESSION_ACK_MESSAGE_BYTES	52
#define SESSION_NACK_MESSAGE_BYTES	52

/* Definition of types of messages */
/* Format:  <2 bits which SM> <4 bits CTR> <1 bit ACK FLAG> <1 bit NACK FLAG> */
#define TYPE_KEP1_SEND  0x04 /* 0b00000100 */
#define TYPE_KEP2_SEND  0x08 /* 0b00001000 */
#define TYPE_KEP3_SEND  0x0C /* 0b00001100 */
#define TYPE_KEP4_SEND  0x10 /* 0b00010000 */
#define TYPE_COMM_SEND  0x40 /* 0b01000000 */
#define TYPE_COMM_ACK   0x42 /* 0b01000010 */
#define TYPE_COMM_NACK  0x41 /* 0b01000001 */
#define TYPE_STAT_SEND  0x80 /* 0b10000000 */
#define TYPE_STAT_ACK   0x82 /* 0b10000010 */
#define TYPE_STAT_NACK  0x81 /* 0b10000001 */
#define TYPE_FEED_SEND  0xC0 /* 0b11000000 */
#define TYPE_FEED_ACK   0xC2 /* 0b11000010 */
#define TYPE_FEED_NACK  0xC1 /* 0b11000001 */
#define TYPE_ALIVE_SEND 0xE0 /* 0b11100000 */
#define TYPE_ALIVE_ACK	0xE2 /* 0b11100010 */
#define TYPE_ALIVE_NACK 0xE1 /* 0b11100001 */

/* Timer and retransmissions and maximal polling times */
#define KEP_RETRANSMISSION_TIMEOUT  	500		/* In milliseconds */
#define KEP_MAX_RETRANSMISSIONS     	10
#define SESSION_RETRANSMISSION_TIMEOUT	500		/* In milliseconds */
#define SESSION_MAX_RETRANSMISSIONS		10
#define MAX_MISSED_SEQNBS				50
#define MIC_PER_MIL						1000

/* Some basic commands */
#define COMMAND_STAT 			"STATUS UPDATE"
#define COMMAND_STAT_LN 		14
#define COMMAND_START_FEED 		"START FEED"
#define COMMAND_START_FEED_LN 	11
#define COMMAND_STOP_FEED 		"STOP FEED"
#define COMMAND_STOP_FEED_LN 	10

/* Global constants */
extern const word ALL_ONE_MASK;
extern const word LEFT_ONE_MASK;

extern const word p[SIZE];
extern const word p_prime[SIZE];
extern const word rp_2[SIZE];
extern const word n[SIZE];
extern const word n_prime[SIZE];
extern const word rn_2[SIZE];
extern const word zero[SIZE];
extern const word one[SIZE];
extern const word one_mont[SIZE];
extern const word a[SIZE];
extern const word a_mont[SIZE];
extern const word b[SIZE];
extern const word b_mont[SIZE];
extern const word c[SIZE];
extern const word g_x[SIZE];
extern const word g_x_mont[SIZE];
extern const word g_y[SIZE];
extern const word g_y_mont[SIZE];

/* Long term keys */
extern const word privDrone[SIZE];
extern const word pkxDrone[SIZE];
extern const word pkyDrone[SIZE];
extern const word privBS[SIZE];
extern const word pkxBS[SIZE];
extern const word pkyBS[SIZE];

#endif