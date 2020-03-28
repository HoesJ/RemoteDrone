#include "platform.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#if UNIX
#include <sys/wait.h>
#include <unistd.h>
#endif
#if WINDOWS
#include <Windows.h>
#include <process.h>
#endif

#ifndef PARAMS_H_
#define PARAMS_H_

/* Stuff for simulation with different processes and pipes */
#if WINDOWS
typedef size_t ssize_t;

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
typedef int32_t  signed_word;
typedef uint32_t word;
typedef uint64_t double_word;
typedef double   float_word;

/* Overall important constants */
#define BITS 				32
#define SIZE 				8
#define BUFFER_SIZE 		128
#define PROC_BIG_ENDIAN		0		/* Why important? when converting a word to bytes. EG for transmitting sequence number */
#define PROC_LITTLE_ENDIAN	1		/* Intel is Little endian -> least significant byte of word is stored at [0] */
#define AIR_BIG_ENDIAN		0		/* Says what standard we use for our air interface */
#define AIR_LITTLE_ENDIAN	1

/* Size of transmitted messages */
#define KEP1_MESSAGE_BYTES  80
#define KEP2_MESSAGE_BYTES  176
#define KEP3_MESSAGE_BYTES  112
#define KEP4_MESSAGE_BYTES  52

/* Definition of types of messages */
/* Format:  <2 bits which SM> <4 bits CTR> <1 bit ACK FLAG> <1 bit NACK FLAG> */
#define TYPE_KEP1_SEND  0b00000100
#define TYPE_KEP2_SEND  0b00001000
#define TYPE_KEP3_SEND  0b00001100
#define TYPE_KEP4_SEND  0b00010000
#define TYPE_COMM_SEND  0b01000000
#define TYPE_COMM_ACK   0b01000010
#define TYPE_COMM_NACK  0b01000001
#define TYPE_STAT_SEND  0b10000000
#define TYPE_STAT_ACK   0b10000010
#define TYPE_STAT_NACK  0b10000001
#define TYPE_FEED_SEND  0b11000000

/* Timer and retransmissions */
#define KEP_RETRANSMISSION_TIMEOUT  30  /* In seconds */
#define KEP_MAX_RETRANSMISSIONS     5

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

#endif