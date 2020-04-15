#include "./../include/enc_dec.h"

/* TODO: check what happens when flag is not yet received. */
/**
 * Polls the receiver pipe.
 * Forms the received message into the fields of decodedMessage struct and
 * sets the status of this message (channel empty, message valid/invalid).
 */
void pollAndDecode(struct SessionInfo* session) {
	word i;
	size_t toRead;
	double_word startTime, elapsedTime;
	ssize_t nbReceived;
	uint32_t length = 0;

	/* Only go on where we left off if the message is not yet complete. */
	if (session->receivedMessage.messageStatus != Message_incomplete) {
		resetCont_IO_ctx(&session->IO);

		/* If the channel was inconsistent the last time, we should first read the garbage. */
		if (session->receivedMessage.messageStatus != Channel_inconsistent) {
			nbReceived = receive(&session->IO, &session->receivedMessage.message, FIELD_TYPE_NB, 0);
			if (nbReceived == -1) {
				session->receivedMessage.messageStatus = Channel_closed;
				return;
			} else if (nbReceived < FIELD_TYPE_NB) {
				session->receivedMessage.messageStatus = Channel_empty;
				return;
			}
		}
	}

	/* Poll the receiver pipe for a maximum amount of time or until you receive a valid message. */
	startTime = (double_word)clock();
	while (1) {
		nbReceived = receive(&session->IO, &session->receivedMessage.message, MAX_MESSAGE_NB + 1, 1);

		if (nbReceived == -1) {
			session->receivedMessage.messageStatus = Channel_closed;
			return;
		} else if (session->IO.endOfMessage) {
			if (session->receivedMessage.messageStatus == Channel_inconsistent) {
				session->receivedMessage.messageStatus = Message_invalid;
				return;
			}

			break;
		} else if (nbReceived == MAX_MESSAGE_NB + 1) {
			/* If you receive more bytes than the maximum in a message, there is a problem. */
			session->receivedMessage.messageStatus = Channel_inconsistent;
			return;
		}

		/* Check if we should still continue. */
		elapsedTime = 1000 * ((float_word)clock() - startTime) / CLOCKS_PER_SEC;
		if (elapsedTime > MAX_POLLING_TIME) {
			session->receivedMessage.messageStatus = Message_incomplete;
			return;
		}
	}

	/* Assign type and length. */
	session->receivedMessage.type = session->receivedMessage.message;
	session->receivedMessage.length = session->receivedMessage.type + FIELD_TYPE_NB;
	memcpy(length, session->receivedMessage.length, FIELD_LENGTH_NB);			/* TODO: Be careful with endianess. */

	switch (*session->receivedMessage.type) {
	case TYPE_KEP1_SEND:
		if (length != KEP1_MESSAGE_BYTES) {
			session->receivedMessage.messageStatus = Message_invalid;
			return;
		} else {
			session->receivedMessage.targetID = session->receivedMessage.length + FIELD_LENGTH_NB;
			session->receivedMessage.seqNb = session->receivedMessage.targetID + FIELD_TARGET_NB;
			session->receivedMessage.data = session->receivedMessage.seqNb + FIELD_SEQNB_NB;
		}
	case TYPE_KEP2_SEND:
		if (length != KEP2_MESSAGE_BYTES) {
			session->receivedMessage.messageStatus = Message_invalid;
			return;
		} else {
			session->receivedMessage.IV = session->receivedMessage.length + FIELD_LENGTH_NB;
			session->receivedMessage.targetID = session->receivedMessage.IV + FIELD_LENGTH_NB;
			session->receivedMessage.seqNb = session->receivedMessage.targetID + FIELD_TARGET_NB;
			session->receivedMessage.data = session->receivedMessage.seqNb + FIELD_SEQNB_NB;
			session->receivedMessage.MAC = session->receivedMessage.message + length - FIELD_MAC_NB;
		}
	case TYPE_KEP3_SEND:
		if (length != KEP3_MESSAGE_BYTES) {
			session->receivedMessage.messageStatus = Message_invalid;
			return;
		} else {
			session->receivedMessage.IV = session->receivedMessage.length + FIELD_LENGTH_NB;
			session->receivedMessage.targetID = session->receivedMessage.IV + FIELD_LENGTH_NB;
			session->receivedMessage.seqNb = session->receivedMessage.targetID + FIELD_TARGET_NB;
			session->receivedMessage.data = session->receivedMessage.seqNb + FIELD_SEQNB_NB;
			session->receivedMessage.MAC = session->receivedMessage.message + length - FIELD_MAC_NB;
		}
	case TYPE_KEP4_SEND:
		if (length != KEP4_MESSAGE_BYTES) {
			session->receivedMessage.messageStatus = Message_invalid;
			return;
		} else {
			session->receivedMessage.IV = session->receivedMessage.length + FIELD_LENGTH_NB;
			session->receivedMessage.targetID = session->receivedMessage.IV + FIELD_LENGTH_NB;
			session->receivedMessage.seqNb = session->receivedMessage.targetID + FIELD_TARGET_NB;
			session->receivedMessage.ackSeqNb = session->receivedMessage.seqNb + FIELD_SEQNB_NB;
			session->receivedMessage.data = session->receivedMessage.seqNb + FIELD_SEQNB_NB;
		}
	}

	/* TODO: endianess!!! */


		/*
		toRead = 0;
#if (ENDIAN_CONVERT)
		for (i = 0; i < FIELD_LENGTH_NB; i++)
			toRead += (1 << (8 * i)) * session->receivedMessage.length[FIELD_LENGTH_NB - 1 - i];
#else
		for (i = 0; i < FIELD_LENGTH_NB; i++)
			toRead += (1 << (8 * i)) * session->receivedMessage.length[i];
#endif*/


}

word validSeqNb(uint8_t* expectedSeqNb, uint8_t* receivedSeqNb) {

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
		else {
			session->expectedSequenceNb[i] = message->seqNb[i] + carry;
			carry = 0;
			iszero = 0;
		}
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
	uint8_t targetID[FIELD_TARGET_NB], uint8_t seqNb[FIELD_SEQNB_NB], uint8_t* IV) {
	message[0] = type;
	memcpy(&message[FIELD_TYPE_NB], length, FIELD_TYPE_NB);
	memcpy(&message[FIELD_TYPE_NB + FIELD_LENGTH_NB], IV, AEGIS_IV_NB);
	memcpy(&message[FIELD_TYPE_NB + FIELD_LENGTH_NB + AEGIS_IV_NB], targetID, FIELD_TARGET_NB);
	memcpy(&message[FIELD_TYPE_NB + FIELD_LENGTH_NB + AEGIS_IV_NB + FIELD_TARGET_NB], seqNb, FIELD_SEQNB_NB);
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