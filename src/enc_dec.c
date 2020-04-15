#include "./../include/enc_dec.h"

/* Increase the given sequence number with one. */
void addOneSeqNb(uint32_t *seqNb) {
	*seqNb = (*seqNb == 0xFFFFFF ? 1 : *seqNb + 1);
}

/**
 * Polls the receiver pipe. The received message is formed into the fields
 * of decodedMessage struct and the status of this message is set. Decrypts
 * and checks the authenticity of the message unless the message is of type
 * TYPE_KEP1_SEND or TYPE_KEP2_SEND.
 */
void pollAndDecode(struct SessionInfo *session) {
	clock_t startTime, elapsedTime;
	ssize_t nbReceived;

	/* If the previous message was incomplete, go on with polling the receiver pipe. Else,
	   reset the IO context. */
	if (session->receivedMessage.messageStatus != Message_incomplete)
		resetCont_IO_ctx(&session->IO);

	/* Poll the receiver pipe for a maximum amount of time or until you receive a valid message. */
	startTime = clock();
	while (1) {
		nbReceived = receive(&session->IO, &session->receivedMessage.message, MAX_MESSAGE_NB + 1, 1);

		/* The pipe was closed. */
		if (nbReceived == -1) {
			session->receivedMessage.messageStatus = Channel_closed;
			return;
		} else if (session->IO.endOfMessage) {
			if (session->receivedMessage.messageStatus == Channel_inconsistent) {
				session->receivedMessage.messageStatus = Message_format_invalid;
				return;
			}

			break;
		} else if (nbReceived == 0) {
			/* Channel should stay inconsistent if it was. */
			if (session->receivedMessage.messageStatus != Channel_inconsistent)
				session->receivedMessage.messageStatus = Channel_empty;
			
			return;
		}
		/* There is a problem if we receive more bytes than the maximum in a message. */
		else if (nbReceived == MAX_MESSAGE_NB + 1) {
			session->receivedMessage.messageStatus = Channel_inconsistent;
			return;
		}

		/* Check if we should still continue. */
		elapsedTime = 1000 * (clock() - startTime) / CLOCKS_PER_SEC;
		if (elapsedTime > MAX_POLLING_TIME) {
			session->receivedMessage.messageStatus = Message_incomplete;
			return;
		}
	}

	/* Assign type and length. */
	session->receivedMessage.type = session->receivedMessage.message;
	session->receivedMessage.length = session->receivedMessage.type + FIELD_TYPE_NB;

	/* Assign length of message in the correct endianness. */
	session->receivedMessage.lengthNum = littleEndianToNum(session->receivedMessage.length);

	/* Determine location of fields based on the type field. */
	switch (*session->receivedMessage.type) {
	case TYPE_KEP1_SEND:
		if (session->receivedMessage.lengthNum != KEP1_MESSAGE_BYTES) {
			session->receivedMessage.messageStatus = Message_format_invalid;
			return;
		} else {
			session->receivedMessage.IV = NULL;
			session->receivedMessage.targetID = session->receivedMessage.length + FIELD_LENGTH_NB;
			session->receivedMessage.seqNb = session->receivedMessage.targetID + FIELD_TARGET_NB;
			session->receivedMessage.ackSeqNb = NULL;
			session->receivedMessage.curvePoint = session->receivedMessage.seqNb + FIELD_SEQNB_NB;
			session->receivedMessage.data = NULL;
			session->receivedMessage.MAC = NULL;
		}
	case TYPE_KEP2_SEND:
		if (session->receivedMessage.lengthNum != KEP2_MESSAGE_BYTES) {
			session->receivedMessage.messageStatus = Message_format_invalid;
			return;
		} else {
			session->receivedMessage.IV = session->receivedMessage.length + FIELD_LENGTH_NB;
			session->receivedMessage.targetID = session->receivedMessage.IV + FIELD_IV_NB;
			session->receivedMessage.seqNb = session->receivedMessage.targetID + FIELD_TARGET_NB;
			session->receivedMessage.ackSeqNb = NULL;
			session->receivedMessage.curvePoint = session->receivedMessage.seqNb + FIELD_SEQNB_NB;
			session->receivedMessage.data = session->receivedMessage.curvePoint + FIELD_CURVEPOINT_NB;
			session->receivedMessage.MAC = session->receivedMessage.message + session->receivedMessage.lengthNum - FIELD_MAC_NB;
		}
	case TYPE_KEP3_SEND:
		if (session->receivedMessage.lengthNum != KEP3_MESSAGE_BYTES) {
			session->receivedMessage.messageStatus = Message_format_invalid;
			return;
		}
		
		session->aegisCtx.iv = session->receivedMessage.message + FIELD_TYPE_NB + FIELD_LENGTH_NB;
		if (!aegisDecryptMessage(&session->aegisCtx, session->receivedMessage.message, FIELD_TYPE_NB + FIELD_LENGTH_NB + FIELD_IV_NB + FIELD_TARGET_NB +
																					   FIELD_SEQNB_NB, FIELD_KEP3_SIGN_NB)) {
			session->receivedMessage.messageStatus = Message_MAC_invalid;
			return;
		} else {
			session->receivedMessage.IV = session->receivedMessage.length + FIELD_LENGTH_NB;
			session->receivedMessage.targetID = session->receivedMessage.IV + FIELD_IV_NB;
			session->receivedMessage.seqNb = session->receivedMessage.targetID + FIELD_TARGET_NB;
			session->receivedMessage.ackSeqNb = NULL;
			session->receivedMessage.curvePoint = NULL;
			session->receivedMessage.data = session->receivedMessage.seqNb + FIELD_SEQNB_NB;
			session->receivedMessage.MAC = session->receivedMessage.message + session->receivedMessage.lengthNum - FIELD_MAC_NB;
		}
	case TYPE_KEP4_SEND:
		if (session->receivedMessage.lengthNum != KEP4_MESSAGE_BYTES) {
			session->receivedMessage.messageStatus = Message_format_invalid;
			return;
		}
		
		session->aegisCtx.iv = session->receivedMessage.message + FIELD_TYPE_NB + FIELD_LENGTH_NB;
		if (!aegisDecryptMessage(&session->aegisCtx, session->receivedMessage.message, FIELD_TYPE_NB + FIELD_LENGTH_NB + FIELD_IV_NB + FIELD_TARGET_NB +
																					   2 * FIELD_SEQNB_NB, 0)) {
			session->receivedMessage.messageStatus = Message_MAC_invalid;
			return;
		} else {
			session->receivedMessage.IV = session->receivedMessage.length + FIELD_LENGTH_NB;
			session->receivedMessage.targetID = session->receivedMessage.IV + FIELD_IV_NB;
			session->receivedMessage.seqNb = session->receivedMessage.targetID + FIELD_TARGET_NB;
			session->receivedMessage.ackSeqNb = session->receivedMessage.seqNb + FIELD_SEQNB_NB;
			session->receivedMessage.curvePoint = NULL;
			session->receivedMessage.data = NULL;
			session->receivedMessage.MAC = session->receivedMessage.message + session->receivedMessage.lengthNum - FIELD_MAC_NB;
		}
	default:
		session->receivedMessage.messageStatus = Message_format_invalid;
		return;
	}

	/* Possibly assign fields in the correct endianness. */
	if (session->receivedMessage.seqNb != NULL)
		session->receivedMessage.seqNbNum = littleEndianToNum(session->receivedMessage.seqNb);

	if (session->receivedMessage.ackSeqNb != NULL)
		session->receivedMessage.ackSeqNbNum = littleEndianToNum(session->receivedMessage.ackSeqNb);
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
	if (!equalByteArrays(&message->seqNb, &session->expectedSequenceNb, FIELD_SEQNB_NB))
		return 0;

	/* Message is OK, increase expected sequence number */
	addOneSeqNb(&session->expectedSequenceNb);

	return 1;
}

/**
 * Prepares a buffer to transmit the message.
 * It populates the buffer with the correct standard fields,
 * but does not transmit the message. It accounts for Endianness when a word
 * needs to be converted to bytes.
 * Returns the offset from where the data needs to be put in
 */
word encodeMessage(uint8_t* message, uint8_t type, uint32_t length,
				   uint8_t targetID[FIELD_TARGET_NB], uint32_t seqNb,
				   uint8_t* IV) {
	uint8_t num[4];

	message[0] = type;
	numToLittleEndian(length, num);
	memcpy(&message[FIELD_TYPE_NB], num, FIELD_TYPE_NB);
	memcpy(&message[FIELD_TYPE_NB + FIELD_LENGTH_NB], IV, AEGIS_IV_NB);
	memcpy(&message[FIELD_TYPE_NB + FIELD_LENGTH_NB + AEGIS_IV_NB], targetID, FIELD_TARGET_NB);
	numToLittleEndian(seqNb, num);
	memcpy(&message[FIELD_TYPE_NB + FIELD_LENGTH_NB + AEGIS_IV_NB + FIELD_TARGET_NB], num, FIELD_SEQNB_NB);

	return FIELD_TYPE_NB + FIELD_LENGTH_NB + FIELD_TARGET_NB + FIELD_SEQNB_NB + AEGIS_IV_NB;
}

/**
 * Prepares a buffer to transmit the message.
 * It populates the buffer with the correct standard fields,
 * but does not transmit the message. It accounts for Endianness when a word
 * needs to be converted to bytes.
 * Returns the offset from where the data needs to be put in
 */
word encodeMessageNoEncryption(uint8_t* message, uint8_t type, uint32_t length,
							   uint8_t targetID[FIELD_TARGET_NB], uint32_t seqNb) {
	uint8_t num[4];

	message[0] = type;
	numToLittleEndian(length, num);
	memcpy(&message[FIELD_TYPE_NB], num, FIELD_TYPE_NB);
	memcpy(&message[FIELD_TYPE_NB + FIELD_LENGTH_NB], targetID, FIELD_TARGET_NB);
	numToLittleEndian(seqNb, num);
	memcpy(&message[FIELD_TYPE_NB + FIELD_LENGTH_NB + FIELD_TARGET_NB], num, FIELD_SEQNB_NB);
	
	return FIELD_TYPE_NB + FIELD_LENGTH_NB + FIELD_TARGET_NB + FIELD_SEQNB_NB;
}