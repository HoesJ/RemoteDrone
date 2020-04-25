#include "./../crypto/aegis.h"
#include "./../general/params.h"

#ifndef SESSION_INFO_H_
#define SESSION_INFO_H_

typedef enum {
    KEP_idle,
    KEP1_compute, KEP1_send, KEP1_wait,
    KEP2_precompute, KEP2_wait_request, KEP2_compute, KEP2_send, KEP2_wait, 
    KEP3_verify, KEP3_compute, KEP3_send, KEP3_wait,
    KEP4_verify, KEP4_compute, KEP4_send, KEP4_wait,
    KEP5_verify,
    Done
} kepState;

/* States have different meaning depening on drone of base station */
typedef enum {
    MESS_idle, MESS_react,
    MESS_encrypt, MESS_verify,
    MESS_send, MESS_wait,
    MESS_ack, MESS_nack
} messState;

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
    Message_repeated,
    Message_invalid,        /* Don't send NACK. */
    Message_used,
	Message_checks_failed,
    Message_incomplete
} messageStatus;

struct State {
    systemState systemState;

    kepState kepState;
    messState commState;
    messState statState;
    messState feedState;
};

struct KEP_ctx {
    word    scalar[SIZE];
	word    generatedPointXY[2 * SIZE];
	word    receivedPointXY[2 * SIZE];
	word	signature[2 * SIZE];

    clock_t timeOfTransmission;
    uint8_t numTransmissions;
	uint32_t sequenceNb;
	uint32_t expectedSequenceNb;

    uint8_t cachedMessage[KEP2_MESSAGE_BYTES];
};

struct MESS_ctx {
	uint8_t sendType;
	uint8_t ackType;
	uint8_t nackType;

	uint32_t sendLength;
	uint32_t nackLength;
	uint32_t ackLength;

    uint8_t needsAcknowledge;
    clock_t timeOfTransmission;
    uint8_t numTransmissions;
	uint32_t sequenceNb;
	uint32_t expectedSequenceNb;

	checkInput  checkInputFunction;
    writeOutput writeOutputFunction;

    uint8_t cachedMessage[MAX_MESSAGE_NB]; /* Also used to store input data! */
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
    /* This array has one byte extra to ensure that the pipe is empty when
       a full message is read. */
    uint8_t message[MAX_MESSAGE_NB + 1];
    
	uint8_t	*type;
	uint8_t	*length;
	uint8_t	*IV;
	uint8_t	*targetID;
	uint8_t	*seqNb;
    uint8_t *ackSeqNb;
    word	*curvePoint;
	uint8_t *data;
    uint8_t *MAC;

    uint32_t lengthNum;
    uint32_t seqNbNum;
    uint32_t ackSeqNbNum;

    messageStatus messageStatus;
};

struct SessionInfo {
	struct AEGIS_ctx		aegisCtx;
	uint8_t					sessionKey[AEGIS_KEY_NB];
    struct IO_ctx			IO;
    struct State			state;
	struct decodedMessage	receivedMessage;

	struct KEP_ctx			kep;
	struct MESS_ctx         comm;
	struct MESS_ctx         stat;
	struct MESS_ctx         feed;
    
    uint8_t					targetID[FIELD_TARGET_NB];
	uint8_t					ownID[FIELD_TARGET_NB];
};

struct externalCommands {
	uint8_t	start;
	uint8_t	quit;
};

#endif