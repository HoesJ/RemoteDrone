#include "params.h"

#ifndef SESSION_INFO_H_

#define SESSION_INFO_H_

typedef enum {
    KEP_idle,
    KEP1_compute, KEP1_send, KEP1_wait,
    KEP2_precompute, KEP2_compute, KEP2_send, KEP2_wait, 
    KEP3_verify, KEP3_compute, KEP3_send, KEP3_wait,
    KEP4_verify, KEP4_compute, KEP4_send,
    KEP5_verify,
    Done
} kepState;

/* States have different meaning depening on drone of base station */
typedef enum {
    COMM_idle,
    COMM_encrypt, COMM_verify,
    COMM_send, COMM_wait, 
    COMM_ack, COMM_nack
} commState;

typedef enum {
    STAT_idle,
    STAT_encrypt, STAT_verify,
    STAT_send, STAT_wait, 
    STAT_ack, STAT_nack
} stateState;

typedef enum {
    FEED_idle,
    FEED_encrypt, FEED_send,
    FEED_verify, FEED_display
} feedState;

typedef enum
{
    Idle,
    ClearSession,
    KEP,
    SessionReady
} systemState;

struct State
{
    systemState systemState;
    kepState kepState;
    commState commState;
    stateState stateState;
    feedState feedState;
};

struct KEP_ctx {
    word    scalar[SIZE];
    word    generatedPointX[SIZE];
    word    generatedPointY[SIZE];
    word    receivedPointX[SIZE];
    word    receivedPointY[SIZE];

    clock_t timeOfTransmission;
    word    numTransmissions;
};

struct IO_ctx {
    int txPipe;
    int rxPipe;
    
    uint8_t buffer[BUFFER_SIZE];
    size_t bufferIndex;
    ssize_t bufferSize;

    word endOfMessage;
    word escRead;

    size_t resIndex;
};

struct SessionInfo {
    struct KEP_ctx  kep;
    uint8_t         sessionKey[16]; /* 128 bit session key*/
    struct IO_ctx   IO;
    struct State    state;
    
    word            targetID;
    word            sequenceNb;
	word			ownID;
	word			expectedSequenceNb;
};

struct externalBaseStationCommands {
	word	start;
	word	quit;
	word	sendCommand;
	word	command;
};
#endif