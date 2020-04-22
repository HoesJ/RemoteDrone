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
 /* TODO: also check received message/remove authenticity check.*/
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
		}
		else if (session->IO.endOfMessage) {
			if (session->receivedMessage.messageStatus == Channel_inconsistent) {
				session->receivedMessage.messageStatus = Message_format_invalid;
				return;
			}

			break;
		}
		else if (nbReceived == 0) {
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

	/* Message is valid until stated otherwise */
	session->receivedMessage.messageStatus = Message_valid;

	/* Assign type and length. */
	session->receivedMessage.type = session->receivedMessage.message;
	session->receivedMessage.length = session->receivedMessage.type + FIELD_TYPE_NB;

	/* Assign length of message in the correct endianness. */
	session->receivedMessage.lengthNum = littleEndianToNum(session->receivedMessage.length, FIELD_LENGTH_NB);

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
		break;
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
			session->receivedMessage.data = ((uint8_t*)session->receivedMessage.curvePoint) + FIELD_CURVEPOINT_NB;
			session->receivedMessage.MAC = session->receivedMessage.message + session->receivedMessage.lengthNum - AEGIS_MAC_NB;
		}
		break;
	case TYPE_KEP3_SEND:
		if (session->receivedMessage.lengthNum != KEP3_MESSAGE_BYTES) {
			session->receivedMessage.messageStatus = Message_format_invalid;
			return;
		} else {
			session->receivedMessage.IV = session->receivedMessage.length + FIELD_LENGTH_NB;
			session->receivedMessage.targetID = session->receivedMessage.IV + FIELD_IV_NB;
			session->receivedMessage.seqNb = session->receivedMessage.targetID + FIELD_TARGET_NB;
			session->receivedMessage.ackSeqNb = NULL;
			session->receivedMessage.curvePoint = NULL;
			session->receivedMessage.data = session->receivedMessage.seqNb + FIELD_SEQNB_NB;
			session->receivedMessage.MAC = session->receivedMessage.message + session->receivedMessage.lengthNum - AEGIS_MAC_NB;
		}
		break;
	case TYPE_KEP4_SEND:
		if (session->receivedMessage.lengthNum != KEP4_MESSAGE_BYTES) {
			session->receivedMessage.messageStatus = Message_format_invalid;
			return;
		} else {
			session->receivedMessage.IV = session->receivedMessage.length + FIELD_LENGTH_NB;
			session->receivedMessage.targetID = session->receivedMessage.IV + FIELD_IV_NB;
			session->receivedMessage.seqNb = session->receivedMessage.targetID + FIELD_TARGET_NB;
			session->receivedMessage.ackSeqNb = session->receivedMessage.seqNb + FIELD_SEQNB_NB;
			session->receivedMessage.curvePoint = NULL;
			session->receivedMessage.data = NULL;
			session->receivedMessage.MAC = session->receivedMessage.message + session->receivedMessage.lengthNum - AEGIS_MAC_NB;
		}
		break;
	case TYPE_COMM_SEND:
	case TYPE_STAT_SEND:
	case TYPE_FEED_SEND:
		if (session->receivedMessage.lengthNum < MIN_MESSAGE_BYTES) {
			session->receivedMessage.messageStatus = Message_format_invalid;
			return;
		} else {
			session->receivedMessage.IV = session->receivedMessage.length + FIELD_LENGTH_NB;
			session->receivedMessage.targetID = session->receivedMessage.IV + FIELD_IV_NB;
			session->receivedMessage.seqNb = session->receivedMessage.targetID + FIELD_TARGET_NB;
			session->receivedMessage.ackSeqNb = NULL;
			session->receivedMessage.curvePoint = NULL;
			session->receivedMessage.data = session->receivedMessage.seqNb + FIELD_SEQNB_NB;
			session->receivedMessage.MAC = session->receivedMessage.message + session->receivedMessage.lengthNum - AEGIS_MAC_NB;
		}
		break;
	case TYPE_COMM_ACK:
	case TYPE_STAT_ACK:
		if (session->receivedMessage.lengthNum != SESSION_ACK_MESSAGE_BYTES) {
			session->receivedMessage.messageStatus = Message_format_invalid;
			return;
		} else {
			session->receivedMessage.IV = session->receivedMessage.length + FIELD_LENGTH_NB;
			session->receivedMessage.targetID = session->receivedMessage.IV + FIELD_IV_NB;
			session->receivedMessage.seqNb = session->receivedMessage.targetID + FIELD_TARGET_NB;
			session->receivedMessage.ackSeqNb = session->receivedMessage.seqNb + FIELD_SEQNB_NB;
			session->receivedMessage.curvePoint = NULL;
			session->receivedMessage.data = NULL;
			session->receivedMessage.MAC = session->receivedMessage.message + session->receivedMessage.lengthNum - AEGIS_MAC_NB;
		}
		break;
	case TYPE_COMM_NACK:
	case TYPE_STAT_NACK:
		if (session->receivedMessage.lengthNum != SESSION_NACK_MESSAGE_BYTES) {
			session->receivedMessage.messageStatus = Message_format_invalid;
			return;
		} else {
			session->receivedMessage.IV = session->receivedMessage.length + FIELD_LENGTH_NB;
			session->receivedMessage.targetID = session->receivedMessage.IV + FIELD_IV_NB;
			session->receivedMessage.seqNb = session->receivedMessage.targetID + FIELD_TARGET_NB;
			session->receivedMessage.ackSeqNb = session->receivedMessage.seqNb + FIELD_SEQNB_NB;
			session->receivedMessage.curvePoint = NULL;
			session->receivedMessage.data = NULL;
			session->receivedMessage.MAC = session->receivedMessage.message + session->receivedMessage.lengthNum - AEGIS_MAC_NB;
		}
		break;
	default:
		session->receivedMessage.messageStatus = Message_format_invalid;
		return;
	}

	/* Possibly assign fields in the correct endianness. */
	if (session->receivedMessage.seqNb != NULL)
		session->receivedMessage.seqNbNum = littleEndianToNum(session->receivedMessage.seqNb, FIELD_SEQNB_NB);

	if (session->receivedMessage.ackSeqNb != NULL)
		session->receivedMessage.ackSeqNbNum = littleEndianToNum(session->receivedMessage.ackSeqNb, FIELD_SEQNB_NB);

	/* Perform basic checks on received message */
	if (!checkReceivedMessage(session, &session->receivedMessage))
		session->receivedMessage.messageStatus = Message_Checks_failed;
}

/**
 * Performs some logic checks on the incomming message.
 * These checks do not include heavy cryptographic stuff, but just
 * target IDs and sequence numbers.
 * If all checks are OK, the expected sequence number is increased.
 * If the message is the first one, the expected sequence number is set
 */
word checkReceivedMessage(struct SessionInfo* session, struct decodedMessage* message) {
	uint32_t maxSeqNb;

	/* Check if we are the target */
	if (!equalByteArrays(message->targetID, session->ownID, FIELD_TARGET_NB))
		return 0;

	/* If this is the first message, remember sequence Nb
	   Make sure that this is both a message that we expect as a first
	   and that we are not holding another sequence number right now.
	   To check if we are not holding a sequence number right now we need
	   to forbid a sequence number value of 0
	   PS, for drone and BS type check should be tailored */
	if (*message->type == TYPE_KEP1_SEND || *message->type == TYPE_KEP2_SEND)
		if (session->expectedSequenceNb == 0 && message->seqNbNum != 0)
			session->expectedSequenceNb = message->seqNbNum;

	/* accept sequence numbers that are equal or higher */
	maxSeqNb = session->expectedSequenceNb + MAX_MISSED_SEQNBS;
	if (((maxSeqNb < session->expectedSequenceNb) && (message->seqNbNum < session->expectedSequenceNb && message->seqNbNum >= maxSeqNb)) ||
		((maxSeqNb > session->expectedSequenceNb) && !(message->seqNbNum >= session->expectedSequenceNb && message->seqNbNum < maxSeqNb)))
		return 0;

	/* We know receiver got the message when it replies with a new seqNb, so then increase expectedSeqNb */
	if (message->seqNbNum != session->expectedSequenceNb)
		session->expectedSequenceNb = message->seqNbNum;

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