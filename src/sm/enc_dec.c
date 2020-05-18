#include "./../../include/sm/enc_dec.h"

/* Increase the given sequence number with one. */
void addOneSeqNb(uint32_t *seqNb) {
	*seqNb = (*seqNb == 0xFFFFFFFF ? 1 : *seqNb + 1);
}

/* Add the given number and sequence number together. */
uint32_t addMultSeqNb(uint32_t seqNb, uint32_t nb) {
	uint32_t res;

	res = seqNb + nb;
	return (res < seqNb ? res + 1 : res);
}

/* Compute seq_num_2 - seq_num_1. */
uint32_t diffSeqNb(uint32_t seq_num_1, uint32_t seq_num_2) {
	uint32_t diff;

	diff = seq_num_2 - seq_num_1;
	if (diff > seq_num_2)
		diff -= 1;

	return diff;
}

/* Implement packet reordering */
static struct decodedMessage video_buffer[NB_CACHED_MESSAGES];
static 		  uint8_t		 video_valid[NB_CACHED_MESSAGES] = { 0 };
static	 	  uint8_t		 nbFreePlaces = NB_CACHED_MESSAGES;

/**
 * Remove all messages from the video feed buffer that have
 * a bad sequence number.
 */
void removeBadMessages(struct SessionInfo* session) {
	uint8_t currentFeedMessage;

	/* Message is bad if distance to expected sequence number is large */
	for (currentFeedMessage = 0; currentFeedMessage < NB_CACHED_MESSAGES; currentFeedMessage++) {
		if (video_valid[currentFeedMessage] && diffSeqNb(session->feed.expectedSequenceNb, video_buffer[currentFeedMessage].seqNbNum) >= MAX_MISSED_SEQNBS) {
			video_valid[currentFeedMessage] = 0;
			nbFreePlaces++;
		}
	}
}

/**
 * Copy a video feed message from the buffer to the received message. If
 * force is set to non-zero, the message with the lowest sequence number
 * is loaded anyway.
 * 
 * Return 1 if message was copied, 0 otherwise.
 */
uint8_t flushVideoBuffer(struct SessionInfo* session, uint8_t force) {
	uint8_t currentFeedMessage;
	uint8_t currentBestFeedMessage = 0xFF;
	uint32_t minDiffFound = 0xFFFFFFFF;

	/* Remove bad messages first */
	removeBadMessages(session);
	
	/* Find feed message with best sequence number */
	for (currentFeedMessage = 0; currentFeedMessage < NB_CACHED_MESSAGES; currentFeedMessage++) {
		if (video_valid[currentFeedMessage]) {
			if (video_buffer[currentFeedMessage].seqNbNum == session->feed.expectedSequenceNb) {
				memcpy(&session->receivedMessage, &video_buffer[currentFeedMessage], sizeof(struct decodedMessage));
				video_valid[currentFeedMessage] = 0;
				nbFreePlaces++;
				return 1;
			} else if (force && diffSeqNb(session->feed.expectedSequenceNb, video_buffer[currentFeedMessage].seqNbNum) <= minDiffFound)
				currentBestFeedMessage = currentFeedMessage;
		}
	}

	/* Copy best feed message found */
	if (force) {
		/* No best message was found, should not happen */
		if (currentBestFeedMessage == 0xFF)
			return 0;
		
		memcpy(&session->receivedMessage, &video_buffer[currentBestFeedMessage], sizeof(struct decodedMessage));
		video_valid[currentBestFeedMessage] = 0;
		nbFreePlaces++;
		return 1;
	} else
		return 0;
}

/**
 * Copy the currently received message to the video buffer. If
 * no places are left in the buffer, the message is not copied.
 * 
 * Return 1 if message was copied, 0 otherwise.
 */
uint8_t copyVideoBuffer(struct SessionInfo* session) {
	uint8_t currentFeedMessage;

	/* Remove bad messages first */
	removeBadMessages(session);

	/* We can't copy if there are no free places. */
	if (nbFreePlaces == 0)
		return 0;

	/* Find number of free places */
	for (currentFeedMessage = 0; currentFeedMessage < NB_CACHED_MESSAGES; currentFeedMessage++) {
		if (!video_valid[currentFeedMessage]) {
			memcpy(&video_buffer[currentFeedMessage], &session->receivedMessage, sizeof(struct decodedMessage));
			video_valid[currentFeedMessage] = 1;
			nbFreePlaces--;
			return 1;
		}
	}

	/* Avoid compiler warnings */
	return 0;
}

/**
 * Polls the receiver pipe. The received message is formed into the fields
 * of decodedMessage struct and the status of this message is set. Decrypts
 * and checks the authenticity of the message unless the message is of type
 * TYPE_KEP1_SEND or TYPE_KEP2_SEND.
 */
void pollAndDecode(struct SessionInfo *session) {
	ssize_t nbReceived;

	/* If the previous message was incomplete, go on with polling the receiver pipe. Else,
	   reset the IO context. */
	if (session->receivedMessage.messageStatus != Message_incomplete)
		resetCont_IO_ctx(&session->IO);

	/* Check if we have received message. */
	nbReceived = receive(&session->IO, session->receivedMessage.message, MAX_MESSAGE_NB + 1, 1);

	/* Check how many bytes were received. */
	if (session->IO.endOfMessage) {
		if (session->receivedMessage.messageStatus == Channel_inconsistent) {
			session->receivedMessage.messageStatus = Message_invalid;
			return;
		} else if (nbReceived == 0) {
			session->receivedMessage.messageStatus = Message_padding;
			return;
		}
	}
	else if (nbReceived == 0) {
		/* Channel should stay inconsistent if it was. */
		if (session->receivedMessage.messageStatus != Channel_inconsistent) {
			/* If channel is empty, we have the possibility to check for video feed messages. */
			session->receivedMessage.messageStatus = Channel_empty;
			flushVideoBuffer(session, 0);
		}

		return;
	}
	/* There is a problem if we receive more bytes than the maximum in a message. */
	else if (nbReceived == MAX_MESSAGE_NB + 1) {
		session->receivedMessage.messageStatus = Channel_inconsistent;
		return;
	} else {
		session->receivedMessage.messageStatus = Message_incomplete;
		return;
	}

	/* Message is valid until stated otherwise */
	session->receivedMessage.messageStatus = Message_valid;

	/* Assign type and length. */
	session->receivedMessage.type = session->receivedMessage.message;
	session->receivedMessage.length = session->receivedMessage.type + FIELD_TYPE_NB;

	/* Assign length of message in the correct endianness. */
	session->receivedMessage.lengthNum = littleEndianToNum(session->receivedMessage.length, FIELD_LENGTH_NB);

	/* Number of received bytes should be consistent */
	if ((nbReceived < FIELD_TYPE_NB + FIELD_LENGTH_NB) || nbReceived != session->receivedMessage.lengthNum) {
		session->receivedMessage.messageStatus = Message_invalid;
		return;
	}

	/* Determine location of fields based on the type field. */
	switch (*session->receivedMessage.type) {
	case TYPE_KEP1_SEND:
		if (session->receivedMessage.lengthNum != KEP1_MESSAGE_BYTES) {
			session->receivedMessage.messageStatus = Message_invalid;
			return;
		} else {
			session->receivedMessage.IV = NULL;
			session->receivedMessage.targetID = session->receivedMessage.length + FIELD_LENGTH_NB;
			session->receivedMessage.seqNb = session->receivedMessage.targetID + FIELD_TARGET_NB;
			session->receivedMessage.ackSeqNb = NULL;
			session->receivedMessage.curvePoint = (word*)(session->receivedMessage.seqNb + FIELD_SEQNB_NB);
			session->receivedMessage.data = NULL;
			session->receivedMessage.MAC = NULL;
		}
		break;
	case TYPE_KEP2_SEND:
		if (session->receivedMessage.lengthNum != KEP2_MESSAGE_BYTES) {
			session->receivedMessage.messageStatus = Message_invalid;
			return;
		} else {
			session->receivedMessage.IV = session->receivedMessage.length + FIELD_LENGTH_NB;
			session->receivedMessage.targetID = session->receivedMessage.IV + FIELD_IV_NB;
			session->receivedMessage.seqNb = session->receivedMessage.targetID + FIELD_TARGET_NB;
			session->receivedMessage.ackSeqNb = NULL;
			session->receivedMessage.curvePoint = (word*)(session->receivedMessage.seqNb + FIELD_SEQNB_NB);
			session->receivedMessage.data = ((uint8_t*)session->receivedMessage.curvePoint) + FIELD_CURVEPOINT_NB;
			session->receivedMessage.MAC = session->receivedMessage.message + session->receivedMessage.lengthNum - AEGIS_MAC_NB;
		}
		break;
	case TYPE_KEP3_SEND:
		if (session->receivedMessage.lengthNum != KEP3_MESSAGE_BYTES) {
			session->receivedMessage.messageStatus = Message_invalid;
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
			session->receivedMessage.messageStatus = Message_invalid;
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
	case TYPE_ALIVE_SEND:
		if (session->receivedMessage.lengthNum < MIN_MESSAGE_BYTES) {
			session->receivedMessage.messageStatus = Message_invalid;
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
	case TYPE_FEED_ACK:
	case TYPE_ALIVE_ACK:
		if (session->receivedMessage.lengthNum != SESSION_ACK_MESSAGE_BYTES) {
			session->receivedMessage.messageStatus = Message_invalid;
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
	case TYPE_FEED_NACK:
	case TYPE_ALIVE_NACK:
		if (session->receivedMessage.lengthNum != SESSION_NACK_MESSAGE_BYTES) {
			session->receivedMessage.messageStatus = Message_invalid;
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
		session->receivedMessage.messageStatus = Message_invalid;
		return;
	}

	/* Possibly assign fields in the correct endianness. */
	if (session->receivedMessage.seqNb != NULL)
		session->receivedMessage.seqNbNum = littleEndianToNum(session->receivedMessage.seqNb, FIELD_SEQNB_NB);

	if (session->receivedMessage.ackSeqNb != NULL)
		session->receivedMessage.ackSeqNbNum = littleEndianToNum(session->receivedMessage.ackSeqNb, FIELD_SEQNB_NB);

	/* Perform basic checks on received message */
	/* This function also sets the status of the received message. */
	checkReceivedMessage(session);
}

/**
 * Performs some logic checks on the incomming message.
 * These checks do not include heavy cryptographic stuff, but just
 * target IDs and sequence numbers.
 * If all checks are OK, the expected sequence number is increased.
 * If the message is the first one, the expected sequence number is set.
 */
void checkReceivedMessage(struct SessionInfo* session) {
	uint32_t maxSeqNb;
	uint32_t *expectedSeqNb;
	struct decodedMessage *message;
	uint8_t currentFeedMessage;

	/* Assign message. */
	message = &session->receivedMessage;

	/* Check if we are the target and the sequence number is valid */
	if (!equalByteArrays(message->targetID, session->ownID, FIELD_TARGET_NB) || message->seqNbNum == 0) {
		message->messageStatus = Message_checks_failed;
		return;
	}

	/* If this is the first message, there is no need to check the expected
	   sequence number. */
	if (*message->type == TYPE_KEP1_SEND || *message->type == TYPE_KEP2_SEND)
		return;

	/* Find expected sequence number */
	switch (*message->type & 0xe0) {
	case TYPE_KEP1_SEND & 0xe0:
		expectedSeqNb = &session->kep.expectedSequenceNb;
		break;
	case TYPE_COMM_SEND & 0xe0:
		expectedSeqNb = &session->comm.expectedSequenceNb;
		break;
	case TYPE_STAT_SEND & 0xe0:
		expectedSeqNb = &session->stat.expectedSequenceNb;
		break;
	case TYPE_FEED_SEND & 0xe0:
		expectedSeqNb = &session->feed.expectedSequenceNb;
		break;
	case TYPE_ALIVE_SEND & 0xe0:
		expectedSeqNb = &session->aliveRes.expectedSequenceNb;
		break;
	default:
		message->messageStatus = Message_checks_failed;
		return;
	}

	/* For video feed, accept sequence numbers that are equal or a little bit higher */
	/* Only do this for unreliable video feed */
	if (*message->type == TYPE_FEED_SEND) {
		maxSeqNb = addMultSeqNb(*expectedSeqNb, MAX_MISSED_SEQNBS);
		if (((maxSeqNb < *expectedSeqNb) && (message->seqNbNum < *expectedSeqNb && message->seqNbNum >= maxSeqNb)) ||
			((maxSeqNb > *expectedSeqNb) && !(message->seqNbNum >= *expectedSeqNb && message->seqNbNum < maxSeqNb))) {
			message->messageStatus = Message_checks_failed;
		} else if (message->seqNbNum == *expectedSeqNb)
			return;
		
		/* Should never happen, but just in case. */
		if (!copyVideoBuffer(session)) {
			/* If copy fails, clear the whole buffer and just return current message. */
			for (currentFeedMessage = 0; currentFeedMessage < NB_CACHED_MESSAGES; currentFeedMessage++)
				video_valid[currentFeedMessage] = 0;
			
			nbFreePlaces = NB_CACHED_MESSAGES;
			return;
		}

		/* If buffer is completely filled, we should display it. */
		if (nbFreePlaces == 0 && flushVideoBuffer(session, 1))
			return;
		
		/* Message should always be invalid at this point */
		message->messageStatus = Message_invalid;
		return;
	}
	/* No guarantees for ACK and NACK messages. */
	else if ((*message->type & 0x03) != 0) {
		return;
	}
	/* For all other packets, we should check if the sequence number is the
	   expected one or one lower. */
	else if (message->seqNbNum == *expectedSeqNb) {
		return;
	} else if (addMultSeqNb(message->seqNbNum, 1) == *expectedSeqNb) {
		message->messageStatus = Message_repeated;
		return;
	} else
		message->messageStatus = Message_checks_failed;
}

/**
 * Prepares a buffer to transmit the message.
 * It populates the buffer with the correct standard fields,
 * but does not transmit the message. It accounts for Endianness when a word
 * needs to be converted to bytes.
 * Returns the offset from where the data needs to be put in
 */
size_t encodeMessage(uint8_t* message, uint8_t type, uint32_t length,
					 uint8_t targetID[FIELD_TARGET_NB], uint32_t seqNb,
					 uint8_t* IV) {
	uint8_t num[4];

	message[0] = type;
	numToLittleEndian(length, num);
	memcpy(&message[FIELD_TYPE_NB], num, FIELD_LENGTH_NB);
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
size_t encodeMessageNoEncryption(uint8_t* message, uint8_t type, uint32_t length,
								 uint8_t targetID[FIELD_TARGET_NB], uint32_t seqNb) {
	uint8_t num[4];

	message[0] = type;
	numToLittleEndian(length, num);
	memcpy(&message[FIELD_TYPE_NB], num, FIELD_LENGTH_NB);
	memcpy(&message[FIELD_TYPE_NB + FIELD_LENGTH_NB], targetID, FIELD_TARGET_NB);
	numToLittleEndian(seqNb, num);
	memcpy(&message[FIELD_TYPE_NB + FIELD_LENGTH_NB + FIELD_TARGET_NB], num, FIELD_SEQNB_NB);

	return FIELD_TYPE_NB + FIELD_LENGTH_NB + FIELD_TARGET_NB + FIELD_SEQNB_NB;
}