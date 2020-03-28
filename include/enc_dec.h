#include "params.h"
#include "session_info.h"
#include "pipe_io.h"
#include "compare_arrays.h"

#ifndef ENC_DEC_H_
#define ENC_DEC_H_

/**
 * Polls the receiver pipe.
 * If the pipe is empty, it returns 0
 * If something comes in, it returns the type of message that has come in.
 * also forms the received message into the fields of decodedMessage struct
 */
uint8_t pollAndDecode(struct SessionInfo* session);

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
word encodeMessage(uint8_t* message, uint8_t type, word length, word targetID, word seqNb, 
	uint8_t* IV, uint8_t* mac, word dataBytes);

/** 
 * Prepares a buffer to transmit the message.
 * It populates the buffer with the correct standard fields, 
 * but does not transmit the message. It accounts for Endianess when a word
 * needs to be converted to bytes.
 */
word encodeMessageNoEncryption(uint8_t* message, uint8_t type, word length, word targetID, word seqNb);

#endif
