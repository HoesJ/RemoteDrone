#include "./../include/enc_dec.h"

/**
 * Convert the endianness of the given array. Both data and result array
 * can be the same.
 */
void convertEndianness(const void *array, void *res, size_t length) {
	size_t currentByte;
	uint8_t tmp;

	for (currentByte = 0; currentByte < length / 2; currentByte++) {
		tmp = ((uint8_t*)array)[currentByte];
		((uint8_t*)res)[currentByte] = ((uint8_t*)array)[length - currentByte - 1];
		((uint8_t*)res)[length - currentByte - 1] = tmp;
	}
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

	/* Possibly convert endianness for length field. */
#ifdef ENDIAN_CONVERT
	convertEndianness(session->receivedMessage.length, &session->receivedMessage.lengthEndian, FIELD_LENGTH_NB);
#else
	memcpy(&session->receivedMessage.lengthEndian, session->receivedMessage.length, FIELD_LENGTH_NB);
#endif

	/* Determine location of fields based on the type field. */
	switch (*session->receivedMessage.type) {
	case TYPE_KEP1_SEND:
		if (*((uint32_t*)session->receivedMessage.lengthEndian) != KEP1_MESSAGE_BYTES) {
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
		if (*((uint32_t*)session->receivedMessage.lengthEndian) != KEP2_MESSAGE_BYTES) {
			session->receivedMessage.messageStatus = Message_format_invalid;
			return;
		} else {
			session->receivedMessage.IV = session->receivedMessage.length + FIELD_LENGTH_NB;
			session->receivedMessage.targetID = session->receivedMessage.IV + FIELD_IV_NB;
			session->receivedMessage.seqNb = session->receivedMessage.targetID + FIELD_TARGET_NB;
			session->receivedMessage.ackSeqNb = NULL;
			session->receivedMessage.curvePoint = session->receivedMessage.seqNb + FIELD_SEQNB_NB;
			session->receivedMessage.data = session->receivedMessage.curvePoint + FIELD_CURVEPOINT_NB;
			session->receivedMessage.MAC = session->receivedMessage.message + *((uint32_t*)session->receivedMessage.lengthEndian) - FIELD_MAC_NB;
		}
	case TYPE_KEP3_SEND:
		if (*((uint32_t*)session->receivedMessage.lengthEndian) != KEP3_MESSAGE_BYTES) {
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
			session->receivedMessage.MAC = session->receivedMessage.message + *((uint32_t*)session->receivedMessage.lengthEndian) - FIELD_MAC_NB;
		}
	case TYPE_KEP4_SEND:
		if (*((uint32_t*)session->receivedMessage.lengthEndian) != KEP4_MESSAGE_BYTES) {
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
			session->receivedMessage.MAC = session->receivedMessage.message + *((uint32_t*)session->receivedMessage.lengthEndian) - FIELD_MAC_NB;
		}
	default:
		session->receivedMessage.messageStatus = Message_format_invalid;
		return;
	}

	/* Possibly convert endianness for sequence number field. */
	if (session->receivedMessage.seqNb != NULL)
#ifdef ENDIAN_CONVERT
		convertEndianness(session->receivedMessage.seqNb, &session->receivedMessage.seqNbEndian, FIELD_SEQNB_NB);
#else
		memcpy(&session->receivedMessage.seqNbEndian, session->receivedMessage.seqNb, FIELD_SEQNB_NB);
#endif

	/* Possibly convert endianness for ACK sequence number field. */
	if (session->receivedMessage.ackSeqNb != NULL)
#ifdef ENDIAN_CONVERT
		convertEndianness(session->receivedMessage.ackSeqNb, &session->receivedMessage.ackSeqNbEndian, FIELD_SEQNB_NB);
#else
		memcpy(&session->receivedMessage.ackSeqNbEndian, session->receivedMessage.ackSeqNb, FIELD_SEQNB_NB);
#endif
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
 * but does not transmit the message. It accounts for Endianness when a word
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
 * but does not transmit the message. It accounts for Endianness when a word
 * needs to be converted to bytes.
 * Returns the offset from where the data needs to be put in
 */
word encodeMessageNoEncryption(uint8_t* message, uint8_t type, uint8_t length,
	uint8_t targetID[FIELD_TARGET_NB], uint8_t seqNb[FIELD_SEQNB_NB]) {
	message[0] = type;
	/* memcpy(&message[FIELD_TYPE_NB], length, FIELD_TYPE_NB); */
	message[FIELD_LENGTH_NB] = length;
	memcpy(&message[FIELD_TYPE_NB + FIELD_LENGTH_NB], targetID, FIELD_TARGET_NB);
	memcpy(&message[FIELD_TYPE_NB + FIELD_LENGTH_NB + FIELD_TARGET_NB], seqNb, FIELD_SEQNB_NB);
	return FIELD_TYPE_NB + FIELD_LENGTH_NB + FIELD_TARGET_NB + FIELD_SEQNB_NB;
}