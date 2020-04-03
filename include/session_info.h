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
} statState;

typedef enum {
    FEED_idle,
    FEED_encrypt, FEED_send,
    FEED_verify, FEED_display
} feedState;

typedef enum {
    Idle,
    ClearSession,
    KEP,
    SessionReady
} systemState;

typedef enum {
    Channel_empty,
    Message_valid,
    Message_invalid
} messageStatus;

struct State {
    systemState systemState;
    kepState kepState;
    commState commState;
    statState statState;
    feedState feedState;
};

struct KEP_ctx {
    word    scalar[SIZE];
    word    generatedPointX[SIZE];
    word    generatedPointY[SIZE];
    word    receivedPointX[SIZE];
    word    receivedPointY[SIZE];

    clock_t timeOfTransmission;
    uint8_t numTransmissions;
};

struct IO_ctx {
    int txPipe;
    int rxPipe;
    
    uint8_t buffer[PIPE_BUFFER_SIZE];
    size_t  bufferIndex;
    ssize_t bufferSize;

    uint8_t endOfMessage;
    uint8_t escRead;

    size_t resIndex;
};

struct decodedMessage {
	uint8_t	type;
	uint8_t	length[FIELD_LENGTH_NB];
	uint8_t	targetID[FIELD_TARGET_NB];
	uint8_t	seqNb[FIELD_SEQNB_NB];
	uint8_t	IV[AEGIS_IV_NB];
	uint8_t MAC[AEGIS_MAC_NB];
	uint8_t data[DECODER_BUFFER_SIZE];

    messageStatus messageStatus;
};

struct SessionInfo {
    struct KEP_ctx			kep;
    uint8_t					sessionKey[AEGIS_KEY_NB];
    struct IO_ctx			IO;
    struct State			state;
	struct decodedMessage	receivedMessage;
    
    uint8_t					targetID[FIELD_TARGET_NB];
	uint8_t					ownID[FIELD_TARGET_NB];
    uint8_t					sequenceNb[FIELD_SEQNB_NB];
	uint8_t					expectedSequenceNb[FIELD_SEQNB_NB];
};

struct externalBaseStationCommands {
	uint8_t	start;
	uint8_t	quit;
	uint8_t	sendCommand;
	uint8_t	command[256];
};

struct externalDroneCommands {
	uint8_t	start;
	uint8_t	quit;
	uint8_t	feedCommand;
    uint8_t updateCommand;
};

#endif