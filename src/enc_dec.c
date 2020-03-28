#include "./../include/enc_dec.h"


#if (AIR_LITTLE_ENDIAN && PROC_LITTLE_ENDIAN) || (AIR_BIG_ENDIAN && PROC_BIG_ENDIAN)
	#define wordToBytes(dest, src, size) {					\
	for (i = 0; i < size; i++)								\
		*(((uint8_t*)dest) + i) = *(((uint8_t*)src) + i);	\
	}
#else
	#define wordToBytes(dest, src, size) {					\
	for (i = 0; i < size; i++)								\
		*(((uint8_t*)dest)+i) = *(((uint8_t*)src)+size-i);	\ /* TODO Check this*/
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
	if (receive(&session->IO, &session->receivedMessage.type, FIELD_TYPE_NB, 0) < FIELD_TYPE_NB)
		return 0;

	/* Read Length */
	for (bytesRead = 0; bytesRead < FIELD_LENGTH_NB; 
		bytesRead += receive(&session->IO, session->receivedMessage.data, FIELD_LENGTH_NB, 1));
	session->receivedMessage.length = session->receivedMessage.data[0];			/* length in practive is only 1 byte so no care for endianess */

	if (session->receivedMessage.type == TYPE_KEP1_SEND) {
		/* Read rest of header */
		for (bytesRead = 0; bytesRead < FIELD_TARGET_NB;
			bytesRead += receive(&session->IO, session->receivedMessage.data, FIELD_TARGET_NB, 1));
		session->receivedMessage.targetID = session->receivedMessage.data[0];	/* targetID in practice is only 1 byte so no care for endianess */
		
		for (bytesRead = 0; bytesRead < FIELD_SEQNB_NB; 
			bytesRead += receive(&session->IO, session->receivedMessage.data, FIELD_SEQNB_NB, 1));
		wordToBytes(&session->receivedMessage.seqNb, session->receivedMessage.data, sizeof(word));

		/* Read data */
		toRead = session->receivedMessage.length - FIELD_TYPE_NB - FIELD_LENGTH_NB - FIELD_TARGET_NB - FIELD_SEQNB_NB;
		for (bytesRead = 0; bytesRead < toRead && !session->IO.endOfMessage;
			bytesRead += receive(&session->IO, session->receivedMessage.data, toRead, 1));
	}
	else {
		/* Read IV */
		for (bytesRead = 0; bytesRead < AEGIS_IV_NB; 
			bytesRead += receive(&session->IO, session->receivedMessage.IV, AEGIS_IV_NB, 1));

		/* Read rest of header */
		for (bytesRead = 0; bytesRead < FIELD_TARGET_NB; 
			bytesRead += receive(&session->IO, session->receivedMessage.data, FIELD_TARGET_NB, 1));
		session->receivedMessage.targetID = session->receivedMessage.data[0];	/* targetID in practice is only 1 byte so no care for endianess */

		for (bytesRead = 0; bytesRead < FIELD_SEQNB_NB; 
			bytesRead += receive(&session->IO, session->receivedMessage.data, FIELD_SEQNB_NB, 1));
		wordToBytes(&session->receivedMessage.seqNb, session->receivedMessage.data, sizeof(word));

		/* Read data */
		toRead = session->receivedMessage.length - FIELD_TYPE_NB - AEGIS_IV_NB - 
			FIELD_LENGTH_NB - FIELD_TARGET_NB - FIELD_SEQNB_NB - AEGIS_MAC_NB;
		for (bytesRead = 0; bytesRead < toRead && !session->IO.endOfMessage; 
			bytesRead += receive(&session->IO, session->receivedMessage.data, toRead, 1));

		/* Read MAC */
		for (bytesRead = 0; bytesRead < AEGIS_MAC_NB && !session->IO.endOfMessage;
			bytesRead += receive(&session->IO, session->receivedMessage.MAC, AEGIS_MAC_NB, 1));
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
	/* Check if we are the target */
	if (message->targetID != session->ownID)
		return 0;

	/* If this is the first message, remember sequence Nb
	   Make sure that this is both a message that we expect as a first 
	   and that we are not holding another sequence number right now.
	   To check if we are not holding a sequence number right now we need
	   to forbid a sequence number value of 0
	   PS, for drone and BS type check should be tailored */
	if ((message->type == TYPE_KEP1_SEND || message->type == TYPE_KEP2_SEND) &&
		session->expectedSequenceNb == 0 && message->seqNb != 0)
		session->expectedSequenceNb = message->seqNb;

	/* Maybe we need some more slack, that higher seqNb's are also OK */
	if (message->seqNb != session->expectedSequenceNb) 
		return 0;

	/* Message is OK, increase expected sequence number */
	session->expectedSequenceNb = message->seqNb + 1 == 0 ? 1 : message->seqNb + 1;
	return 1;
}

/**
 * Prepares a buffer to transmit the message.
 * It populates the buffer with the correct standard fields,
 * but does not transmit the message. It accounts for Endianess when a word
 * needs to be converted to bytes.
 */
void encodeMessage(uint8_t* message, uint8_t type, word length, word targetID, word seqNb,
	uint8_t* IV, uint8_t* mac, word numDataBytes) {

	word i;

	message[0] = type;
	message[FIELD_TYPE_NB] = length;
	memcpy(&message[FIELD_TYPE_NB+FIELD_LENGTH_NB], IV, AEGIS_IV_NB);
	message[FIELD_TYPE_NB+FIELD_LENGTH_NB +AEGIS_IV_NB] = targetID;

	wordToBytes(&message[FIELD_TYPE_NB + FIELD_LENGTH_NB + FIELD_TARGET_NB + AEGIS_IV_NB], &seqNb, sizeof(word));

	memcpy(&message[FIELD_TYPE_NB + FIELD_LENGTH_NB + FIELD_TARGET_NB + FIELD_SEQNB_NB + AEGIS_IV_NB + numDataBytes], mac, AEGIS_MAC_NB);

}

/**
 * Prepares a buffer to transmit the message.
 * It populates the buffer with the correct standard fields,
 * but does not transmit the message. It accounts for Endianess when a word
 * needs to be converted to bytes.
 */
void encodeMessageNoEncryption(uint8_t* message, uint8_t type, word length, word targetID, word seqNb) {
	word i;

	message[0] = type;
	message[FIELD_TYPE_NB] = length;
	message[FIELD_TYPE_NB + FIELD_LENGTH_NB] = targetID;

	wordToBytes(&message[FIELD_TYPE_NB + FIELD_LENGTH_NB + FIELD_TARGET_NB], &seqNb, sizeof(word));
}