#include "./../include/enc_dec.h"


#if (AIR_LITTLE_ENDIAN && PROC_LITTLE_ENDIAN) || (AIR_BIG_ENDIAN && PROC_BIG_ENDIAN)
#define wordToBytes(dest, src, size) {					\
	for (i = 0; i < size; i++)								\
		*(((uint8_t*)dest) + i) = *(((uint8_t*)src) + i);	\
	}
#else
#define wordToBytes(dest, src, size) {					\
	for (i = 0; i < size; i++)								\
		*(((uint8_t*)dest) + i) = *(((uint8_t*)src)+size-i-1);\
	}
#endif

/**
 * Polls the receiver pipe.
 * If the pipe is empty, it returns 0
 * If something comes in, it returns the type of message that has come in.
 * also forms the received message into the fields of decodedMessage struct
 */
uint8_t pollAndDecode(struct SessionInfo* session) {
	word bytesRead;
	word i;
	word toRead;

	/* Poll the pipe for Type byte */
	resetCont_IO_ctx(&session->IO);
	if (receive(&session->IO, &session->receivedMessage.type, FIELD_TYPE_NB, 0) < FIELD_TYPE_NB)
		return 0;

	/* Read Length */
	resetCont_IO_ctx(&session->IO);
	while (receive(&session->IO, session->receivedMessage.length, FIELD_LENGTH_NB, 1) < FIELD_LENGTH_NB);

	if (session->receivedMessage.type == TYPE_KEP1_SEND) {
		/* Read rest of header */
		resetCont_IO_ctx(&session->IO);
		while (receive(&session->IO, session->receivedMessage.targetID, FIELD_TARGET_NB, 1) < FIELD_TARGET_NB);

		resetCont_IO_ctx(&session->IO);
		while (receive(&session->IO, session->receivedMessage.seqNb, FIELD_SEQNB_NB, 1) < FIELD_SEQNB_NB);

		/* Read data */
		toRead = session->receivedMessage.length - FIELD_TYPE_NB - FIELD_LENGTH_NB - FIELD_TARGET_NB - FIELD_SEQNB_NB;
		resetCont_IO_ctx(&session->IO);
		while (receive(&session->IO, session->receivedMessage.data, toRead, 1) < toRead);
	}
	else {
		/* Read IV */
		resetCont_IO_ctx(&session->IO);
		while (receive(&session->IO, session->receivedMessage.IV, AEGIS_IV_NB, 1) < AEGIS_IV_NB);

		/* Read rest of header */
		resetCont_IO_ctx(&session->IO);
		while (receive(&session->IO, session->receivedMessage.targetID, FIELD_TARGET_NB, 1) < FIELD_TARGET_NB);

		resetCont_IO_ctx(&session->IO);
		while (receive(&session->IO, session->receivedMessage.seqNb, FIELD_SEQNB_NB, 1) < FIELD_SEQNB_NB);

		/* Read data */
		toRead = session->receivedMessage.length - FIELD_TYPE_NB - AEGIS_IV_NB -
			FIELD_LENGTH_NB - FIELD_TARGET_NB - FIELD_SEQNB_NB - AEGIS_MAC_NB;
		resetCont_IO_ctx(&session->IO);
		while (!session->IO.endOfMessage &&
			receive(&session->IO, session->receivedMessage.data, toRead, 1) < toRead);

		/* Read MAC */
		resetCont_IO_ctx(&session->IO);
		while (!session->IO.endOfMessage &&
			receive(&session->IO, session->receivedMessage.MAC, AEGIS_MAC_NB, 1) < AEGIS_MAC_NB);
	}
	return session->receivedMessage.type;
}

/**
 * Performs some logic checks on the incomming message.
 * These checks do not include heavy cryptographic stuff, but just
 * target IDs and sequence numbers.
 * If all checks are OK, the expected sequence number is increased.
 * If the message is the first one, the expected sequence number is set
 */
word checkReceivedMessage(struct SessionInfo* session, struct decodedMessage* message) {
	word i, carry, iszero;

	/* Check if we are the target */
	if (!equalByteArrays(message->targetID, session->ownID, FIELD_TARGET_NB))
		return 0;

	/* If this is the first message, remember sequence Nb
	   Make sure that this is both a message that we expect as a first
	   and that we are not holding another sequence number right now.
	   To check if we are not holding a sequence number right now we need
	   to forbid a sequence number value of 0
	   PS, for drone and BS type check should be tailored */
	if (message->type == TYPE_KEP1_SEND || message->type == TYPE_KEP2_SEND)
		if (isZero(session->expectedSequenceNb, FIELD_SEQNB_NB) &&
			!isZero(message->seqNb, FIELD_SEQNB_NB))
			memcpy(session->expectedSequenceNb, message->seqNb, FIELD_SEQNB_NB);


	/* Maybe we need some more slack, that higher seqNb's are also OK */
	if (!equalByteArrays(message->seqNb, session->expectedSequenceNb, FIELD_SEQNB_NB))
		return 0;

	/* Message is OK, increase expected sequence number */
	carry = 1; iszero = 1;
	for (i = 0; i < FIELD_SEQNB_NB; i++) {
		if (message->seqNb[i] == 0xff) {
			session->expectedSequenceNb[i] = 0x00;
			carry = 1;
		}
		session->expectedSequenceNb[i] = message->seqNb[i] + carry;
		carry = 0;
		iszero = 0;
	}
	if (iszero)
		session->expectedSequenceNb[0] = 0x01;
	
	return 1;
}

/**
 * Prepares a buffer to transmit the message.
 * It populates the buffer with the correct standard fields,
 * but does not transmit the message. It accounts for Endianess when a word
 * needs to be converted to bytes.
 * Returns the offset from where the data needs to be put in
 */
word encodeMessage(uint8_t* message, uint8_t type, uint8_t length[FIELD_LENGTH_NB],
	uint8_t targetID[FIELD_TARGET_NB], uint8_t seqNb[FIELD_SEQNB_NB],
	uint8_t* IV, uint8_t* mac, word numDataBytes) {

	message[0] = type;
	memcpy(&message[FIELD_TYPE_NB], length, FIELD_TYPE_NB);
	memcpy(&message[FIELD_TYPE_NB + FIELD_LENGTH_NB], IV, AEGIS_IV_NB);
	memcpy(&message[FIELD_TYPE_NB + FIELD_LENGTH_NB + AEGIS_IV_NB], targetID, FIELD_TARGET_NB);
	memcpy(&message[FIELD_TYPE_NB + FIELD_LENGTH_NB + AEGIS_IV_NB + FIELD_TARGET_NB], seqNb, FIELD_SEQNB_NB);
	memcpy(&message[FIELD_TYPE_NB + FIELD_LENGTH_NB + FIELD_TARGET_NB + FIELD_SEQNB_NB + AEGIS_IV_NB + numDataBytes], mac, AEGIS_MAC_NB);
	return FIELD_TYPE_NB + FIELD_LENGTH_NB + FIELD_TARGET_NB + FIELD_SEQNB_NB + AEGIS_IV_NB;
}

/**
 * Prepares a buffer to transmit the message.
 * It populates the buffer with the correct standard fields,
 * but does not transmit the message. It accounts for Endianess when a word
 * needs to be converted to bytes.
 * Returns the offset from where the data needs to be put in
 */
word encodeMessageNoEncryption(uint8_t* message, uint8_t type, uint8_t length[FIELD_LENGTH_NB],
	uint8_t targetID[FIELD_TARGET_NB], uint8_t seqNb[FIELD_SEQNB_NB]) {

	message[0] = type;
	memcpy(&message[FIELD_TYPE_NB], length, FIELD_TYPE_NB);
	memcpy(&message[FIELD_TYPE_NB + FIELD_LENGTH_NB], targetID, FIELD_TARGET_NB);
	memcpy(&message[FIELD_TYPE_NB + FIELD_LENGTH_NB + FIELD_TARGET_NB], seqNb, FIELD_SEQNB_NB);
	return FIELD_TYPE_NB + FIELD_LENGTH_NB + FIELD_TARGET_NB + FIELD_SEQNB_NB;
}