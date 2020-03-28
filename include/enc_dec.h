#include "params.h"
#include "session_info.h"
#include "pipe_io.h"

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
 * Prepares a buffer to transmit the message.
 * It populates the buffer with the correct standard fields,
 * but does not transmit the message. It accounts for Endianess when a word
 * needs to be converted to bytes.
 */
void encodeMessage(uint8_t* message, uint8_t type, word length, word targetID, word seqNb, 
	uint8_t* IV, uint8_t* mac, uint8_t* data, word dataBytes);

/** 
 * Prepares a buffer to transmit the message.
 * It populates the buffer with the correct standard fields, 
 * but does not transmit the message. It accounts for Endianess when a word
 * needs to be converted to bytes.
 */
void encodeMessageNoEncryption(uint8_t* message, uint8_t type, word length, word targetID, word seqNb, 
	uint8_t* data, word dataBytes);

#endif
