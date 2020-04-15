#include "params.h"
#include "aegis.h"

#ifndef SESSION_INFO_H_
#define SESSION_INFO_H_

typedef enum {
    KEP_idle,
    KEP1_compute, KEP1_send, KEP1_wait,
    KEP2_precompute, KEP2_compute, KEP2_send, KEP2_wait, 
    KEP3_verify, KEP3_compute, KEP3_send, KEP3_wait,
    KEP4_verify, KEP4_compute, KEP4_send, KEP4_wait,
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
    Channel_closed,
    Channel_inconsistent,
    Message_valid,
    Message_format_invalid,     /* Don't send NACK. */
    Message_MAC_invalid,        /* Should send NACK if sequence number makes sense. */
    Message_incomplete
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
	word    generatedPointXY[2 * SIZE];
	word    receivedPointXY[2 * SIZE];
	word	signature[2 * SIZE];

    clock_t timeOfTransmission;
    uint8_t numTransmissions;

    uint8_t cachedMessage[KEP2_MESSAGE_BYTES];
    uint8_t cachedMessageValid;
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
    uint8_t message [MAX_MESSAGE_NB + 1];       /* +1 so that you can ensure that the pipe is empty. */
    
	uint8_t	*type;
	uint8_t	*length;
	uint8_t	*IV;
	uint8_t	*targetID;
	uint8_t	*seqNb;
    uint8_t *ackSeqNb;
    uint8_t *curvePoint;
	uint8_t *data;
    uint8_t *MAC;

    uint32_t lengthNum;
    uint32_t seqNbNum;
    uint32_t ackSeqNbNum;

    messageStatus messageStatus;    /* Needs to be initialized!!! */
};

struct SessionInfo {
	struct AEGIS_ctx		aegisCtx;
	uint8_t					sessionKey[AEGIS_KEY_NB];
    struct KEP_ctx			kep;
    struct IO_ctx			IO;
    struct State			state;
	struct decodedMessage	receivedMessage;
    
    uint8_t					targetID[FIELD_TARGET_NB];
	uint8_t					ownID[FIELD_TARGET_NB];
    uint32_t				sequenceNb;
	uint32_t				expectedSequenceNb;
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