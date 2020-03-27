#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

#ifndef PARAMS_H_
#define PARAMS_H_

#define BUFFER_SIZE 128

typedef int32_t  signed_word;
typedef uint32_t word;
typedef uint64_t double_word;
typedef double   float_word;

extern const word ALL_ONE_MASK;
extern const word LEFT_ONE_MASK;

#define BITS 32
#define SIZE 8

#define KEP1_MESSAGE_BYTES  80
#define KEP2_MESSAGE_BYTES  176
#define KEP3_MESSAGE_BYTES  112
#define KEP4_MESSAGE_BYTES  52

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

#define KEP_RETRANSMISSION_TIMEOUT  30  /* In seconds */
#define KEP_MAX_RETRANSMISSIONS     5

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