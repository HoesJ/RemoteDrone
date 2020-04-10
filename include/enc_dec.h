#include "params.h"
#include "session_info.h"
#include "pipe_io.h"
#include "compare_arrays.h"

#ifndef ENC_DEC_H_
#define ENC_DEC_H_

/**
 * Polls the receiver pipe.
 * Forms the received message into the fields of decodedMessage struct and
 * sets the status of this message (channel empty, message valid/invalid).
 */
void pollAndDecode(struct SessionInfo* session);

/**
 * Performs some logic checks on the incomming message.
 * These checks do not include heavy cryptographic stuff, but just
 * target IDs and sequence numbers.
 * If all checks are OK, the expected sequence number is increased.
 * If the message is the first one, the expected sequence number is set
 */
word checkReceivedMessage(struct SessionInfo* session, struct decodedMessage* message);

/**
 * Prepares a buffer to transmit the message.
 * It populates the buffer with the correct standard fields,
 * but does not transmit the message. It accounts for Endianess when a word
 * needs to be converted to bytes.
 */
word encodeMessage(uint8_t* message, uint8_t type, uint8_t length[FIELD_LENGTH_NB],
				   uint8_t targetID[FIELD_TARGET_NB], uint8_t seqNb[FIELD_SEQNB_NB],
				   uint8_t* IV);

/** 
 * Prepares a buffer to transmit the message.
 * It populates the buffer with the correct standard fields, 
 * but does not transmit the message. It accounts for Endianess when a word
 * needs to be converted to bytes.
 */
word encodeMessageNoEncryption(uint8_t* message, uint8_t type, uint8_t length,
							   uint8_t targetID[FIELD_TARGET_NB], uint8_t seqNb[FIELD_SEQNB_NB]);

/**
 * Converts a word array to a byte array and the other way around. Input and output cannot be equal.
 */
#if (!ENDIAN_CONVERT) /* Equals so simply copy without care */
#define wordArrayToByteArray(dest, src, nbBytes) {	\
		memcpy((dest), (src), nbBytes);				\
	}
#else
#define wordArrayToByteArray(dest, src, nbBytes) {		\
	for (i = 0; i < nbBytes; i += sizeof(word))			\
		for (j = 0; j < sizeof(word); j++)				\
			*(((uint8_t*)dest) + sizeof(word) * i + j) = *(((uint8_t*)src) + sizeof(word) * (i + 1) - j - 1); \
	}
#endif

#endif