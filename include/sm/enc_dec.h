#include "./../general/params.h"
#include "./../general/compare_arrays.h"
#include "./../general/endian_convert.h"
#include "./../sm/session_info.h"
#include "./../sm/udp_io.h"

#ifndef ENC_DEC_H_
#define ENC_DEC_H_

/* Increase the given sequence number with one. */
void addOneSeqNb(uint32_t *seqNb);

/* Add the given number and sequence number together. */
uint32_t addMultSeqNb(uint32_t seqNb, uint32_t nb);

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
void checkReceivedMessage(struct SessionInfo* session);

/**
 * Prepares a buffer to transmit the message.
 * It populates the buffer with the correct standard fields,
 * but does not transmit the message. It accounts for Endianness when a word
 * needs to be converted to bytes.
 * Returns the offset from where the data needs to be put in
 */
size_t encodeMessage(uint8_t* message, uint8_t type, uint32_t length,
				     uint8_t targetID[FIELD_TARGET_NB], uint32_t seqNb,
				     uint8_t* IV);

/**
 * Prepares a buffer to transmit the message.
 * It populates the buffer with the correct standard fields,
 * but does not transmit the message. It accounts for Endianness when a word
 * needs to be converted to bytes.
 * Returns the offset from where the data needs to be put in
 */
size_t encodeMessageNoEncryption(uint8_t* message, uint8_t type, uint32_t length,
							     uint8_t targetID[FIELD_TARGET_NB], uint32_t seqNb);

#endif