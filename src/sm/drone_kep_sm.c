#include "./../../include/sm/drone_kep_sm.h"

/**
 * 0: stay in KEP_idle
 * 1: go to KEP2_precompute
 */
int8_t KEP_idle_handlerDrone(struct SessionInfo* session) {
	init_KEP_ctxDrone(&session->kep);
	return 1;
}

/**
 * 0: stay in KEP2_precompute
 * 1: go to KEP2_wait_request
 */
int8_t KEP2_precompute_handlerDrone(struct SessionInfo* session) {
	ECDHGenerateRandomSample(session->kep.scalar, session->kep.generatedPointXY, session->kep.generatedPointXY + SIZE);
	return 1;
}

/**
 * 0: stay in KEP2_wait_request
 * 1: go to KEP2_compute
 */
int8_t KEP2_wait_request_handlerDrone(struct SessionInfo* session) {
	if (session->receivedMessage.messageStatus == Message_valid || session->receivedMessage.messageStatus == Message_repeated) {
		if (*session->receivedMessage.type == TYPE_KEP1_SEND)
			return 1;
		else
			session->receivedMessage.messageStatus = Message_used;
	}

	return 0;
}

/**
 * 0: stay in KEP2_compute
 * 1: go to KEP2_send
 */
int8_t KEP2_compute_handlerDrone(struct SessionInfo* session) {
	word index;
	uint32_t length;
	uint8_t IV[AEGIS_IV_NB];

	word XYout[2 * SIZE];
	word concatPoints[4 * SIZE];

	/* This function will use the received message. */
	session->receivedMessage.messageStatus = Message_used;

	/* Set expected sequence number. */
	session->kep.expectedSequenceNb = addMultSeqNb(session->receivedMessage.seqNbNum, 1);

	/* Copy received point to state. */
	memcpy(session->kep.receivedPointXY, session->receivedMessage.curvePoint, 2 * SIZE * sizeof(word));

	/* Compute resulting point on elliptic curve. */
	ECDHPointMultiply(session->kep.scalar, session->kep.receivedPointXY, session->kep.receivedPointXY + SIZE, XYout, XYout + SIZE);

	/* Compute session key. */
	sha3_HashBuffer(256, SHA3_FLAGS_NONE, XYout, 2 * SIZE * sizeof(word), session->sessionKey, AEGIS_KEY_NB);
	init_AEGIS_ctx(&session->aegisCtx, session->sessionKey);

	/* Compute signature. */
	memcpy(concatPoints, session->kep.generatedPointXY, 2 * SIZE * sizeof(word));
	memcpy(concatPoints + 2 * SIZE, session->kep.receivedPointXY, 2 * SIZE * sizeof(word));
	ecdsaSign(concatPoints, 4 * SIZE * sizeof(word), privDrone, session->kep.signature, session->kep.signature + SIZE);

	length = KEP2_MESSAGE_BYTES;
	getRandomBytes(AEGIS_IV_NB, IV);
	addOneSeqNb(&session->kep.sequenceNb);
	index = encodeMessage(session->kep.cachedMessage, TYPE_KEP2_SEND, length, session->targetID, session->kep.sequenceNb, IV);

	/* Put data in */
	memcpy(session->kep.cachedMessage + index, session->kep.generatedPointXY, 2 * SIZE * sizeof(word));
	memcpy(session->kep.cachedMessage + index + FIELD_CURVEPOINT_NB, session->kep.signature, 2 * SIZE * sizeof(word));

	/* Encrypt */
	session->aegisCtx.iv = IV;
	aegisEncryptMessage(&session->aegisCtx, session->kep.cachedMessage, index + FIELD_CURVEPOINT_NB, FIELD_SIGN_NB);

	return 1;
}

/**
 * 0: go to KEP_idle
 * 1: go to KEP2_wait
 */
int8_t KEP2_send_handlerDrone(struct SessionInfo* session) {
	if (session->kep.numTransmissions >= KEP_MAX_RETRANSMISSIONS) {
		/* Abort KEP. */
		printf("Drone\t- Max retransmissions reached on %x\n", session->kep.cachedMessage[0]);
		session->state.systemState = ClearSession;
		return 0;
	}

	/* Send message */
	transmit(&session->IO, session->kep.cachedMessage, KEP2_MESSAGE_BYTES, 1);

	/* Manage administration */
	session->kep.numTransmissions++;
	session->kep.timeOfTransmission = clock();

	return 1;
}

/**
 * -1: go to KEP2_send
 *  0: stay in KEP2_wait
 *  1: go to KEP4_verify
 *  2: go to KEP2_compute
 */
int8_t KEP2_wait_handlerDrone(struct SessionInfo* session) {
	double_word currentTime;
	double_word elapsedTime;

	if (session->receivedMessage.messageStatus == Message_valid || session->receivedMessage.messageStatus == Message_repeated) {
		if (*session->receivedMessage.type == TYPE_KEP3_SEND) {
			return 1;
		} else if (*session->receivedMessage.type == TYPE_KEP1_SEND) {
			session->receivedMessage.messageStatus = Message_used;
			return 2;
		} else {
			session->receivedMessage.messageStatus = Message_used;
			return -1;
		}
	}

	currentTime = (double_word)clock();
	elapsedTime = ((float_word)currentTime - session->kep.timeOfTransmission) / CLOCKS_PER_SEC;
	if (elapsedTime > KEP_RETRANSMISSION_TIMEOUT)
		return -1;
	else
		return 0;
}

/**
 * 0: go to KEP2_send
 * 1: go to KEP4_compute
 */
int8_t KEP4_verify_handlerDrone(struct SessionInfo* session) {
	uint8_t correct;
	uint8_t messageToSign[4 * SIZE * sizeof(word)];

	/* This function will use the received message. */
	session->receivedMessage.messageStatus = Message_used;

	/* MAC and Decryption */
	session->aegisCtx.iv = session->receivedMessage.IV;
	correct = aegisDecryptMessage(&session->aegisCtx, session->receivedMessage.message,
								  session->receivedMessage.data - session->receivedMessage.type, FIELD_SIGN_NB);
	if (!correct)
		return 0;

	/* Check signature */
	memcpy(messageToSign, session->kep.receivedPointXY, 2 * SIZE * sizeof(word));
	memcpy(messageToSign + 2 * SIZE * sizeof(word), session->kep.generatedPointXY, 2 * SIZE * sizeof(word));

	correct = ecdsaCheck(messageToSign, 4 * SIZE * sizeof(word), pkxBS, pkyBS,
						 session->receivedMessage.data, session->receivedMessage.data + SIZE * sizeof(word));

	/* Manage administration */
	if (correct) {
		session->kep.numTransmissions 	= 0;
		session->kep.timeOfTransmission = 0;

		session->kep.expectedSequenceNb = addMultSeqNb(session->receivedMessage.seqNbNum, 1);
	}

	return correct;
}

/**
 * 0: stay in KEP4_compute
 * 1: go to KEP4_send
 */
int8_t KEP4_compute_handlerDrone(struct SessionInfo* session) {
	size_t index;
	uint32_t length;
	uint8_t IV[AEGIS_IV_NB];

	length = KEP4_MESSAGE_BYTES;
	getRandomBytes(AEGIS_IV_NB, IV);
	addOneSeqNb(&session->kep.sequenceNb);
	index = encodeMessage(session->kep.cachedMessage, TYPE_KEP4_SEND, length, session->targetID, session->kep.sequenceNb, IV);

	/* Put data in */
	memcpy(session->kep.cachedMessage + index, session->receivedMessage.seqNb, FIELD_SEQNB_NB);
	
	/* Encrypt */
	session->aegisCtx.iv = IV;
	aegisEncryptMessage(&session->aegisCtx, session->kep.cachedMessage, index + FIELD_SEQNB_NB, 0);

	return 1;
}

/**
 * 0: go to KEP_idle
 * 1: go to Done
 */
int8_t KEP4_send_handlerDrone(struct SessionInfo* session) {
	if (session->kep.numTransmissions >= KEP_MAX_RETRANSMISSIONS) {
		/* Abort KEP. */
		printf("Drone\t- Max retransmissions reached on %x\n", session->kep.cachedMessage[0]);
		session->state.systemState = ClearSession;
		return 0;
	}

	/* Send message */
	transmit(&session->IO, session->kep.cachedMessage, KEP4_MESSAGE_BYTES, 1);

	/* Manage administration */
	session->kep.numTransmissions++;
	session->kep.timeOfTransmission = clock();

	return 1;
}

/**
 * 0: go to KEP_idle
 * 1: stay in Done
 */
int8_t Done_handler(struct SessionInfo* session) {
	uint8_t correct;

	/* This function will use the received message. */
	session->receivedMessage.messageStatus = Message_used;

	/* MAC and Decryption */
	session->aegisCtx.iv = session->receivedMessage.IV;
	correct = aegisDecryptMessage(&session->aegisCtx, session->receivedMessage.message,
								  session->receivedMessage.data - session->receivedMessage.type, FIELD_SIGN_NB);
	if (!correct)
		return 1;
	
	return KEP4_send_handlerDrone(session);
}

/* Public functions */

void init_KEP_ctxDrone(struct KEP_ctx* ctx) {
	ctx->timeOfTransmission = 0;
	ctx->numTransmissions = 0;

	getRandomBytes(FIELD_SEQNB_NB, &ctx->sequenceNb);
	ctx->expectedSequenceNb = 0;

	/* Set arrays to zero */
	memset(ctx->scalar, 0, sizeof(word) * SIZE);
	memset(ctx->generatedPointXY, 0, 2 * sizeof(word) * SIZE);
	memset(ctx->receivedPointXY, 0, 2 * sizeof(word) * SIZE);
	memset(ctx->signature, 0, 2 * sizeof(word) * SIZE);
	memset(ctx->cachedMessage, 0, KEP2_MESSAGE_BYTES);
}

kepState kepContinueDrone(struct SessionInfo* session, kepState currentState) {
	switch (currentState) {
	case KEP_idle:
		return KEP_idle_handlerDrone(session) ? KEP2_precompute : KEP_idle;

	case KEP2_precompute:
		return KEP2_precompute_handlerDrone(session) ? KEP2_wait_request : KEP2_precompute;

	case KEP2_wait_request:
		return KEP2_wait_request_handlerDrone(session) ? KEP2_compute : KEP2_wait_request;

	case KEP2_compute:
		return KEP2_compute_handlerDrone(session) ? KEP2_send : KEP2_compute;

	case KEP2_send:
		return KEP2_send_handlerDrone(session) ? KEP2_wait : KEP_idle;

	case KEP2_wait:
		switch (KEP2_wait_handlerDrone(session)) {
		case -1: return KEP2_send;
		case 0:  return KEP2_wait;
		case 1:  return KEP4_verify;
		case 2:  return KEP2_compute;
		default: return KEP_idle;
		}

	case KEP4_verify:
		return KEP4_verify_handlerDrone(session) ? KEP4_compute : KEP2_send;
	
	case KEP4_compute:
		return KEP4_compute_handlerDrone(session) ? KEP4_send : KEP4_compute;

	case KEP4_send:
		return KEP4_send_handlerDrone(session) ? Done : KEP_idle;

	case Done:
		return Done_handler(session) ? Done : KEP_idle;

	default:
		return KEP_idle;
	}
}