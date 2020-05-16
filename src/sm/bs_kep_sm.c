#include "./../../include/sm/bs_kep_sm.h"

/**
 * 0: go to KEP_idle
 * 1: go to KEP1_compute
 */
int8_t KEP1_idle_handlerBaseStation(struct SessionInfo* session) {
	init_KEP_ctxBaseStation(&session->kep);
	return 1;
}

/**
 * 0: stay in KEP1_compute
 * 1: go to KEP1_send
 */
int8_t KEP1_compute_handlerBaseStation(struct SessionInfo* session) {
	size_t index;
	uint32_t length;

	ECDHGenerateRandomSample(session->kep.scalar, session->kep.generatedPointXY, session->kep.generatedPointXY + SIZE);

	/* Form message */
	length = KEP1_MESSAGE_BYTES;
	addOneSeqNb(&session->kep.sequenceNb);
	index = encodeMessageNoEncryption(session->kep.cachedMessage, TYPE_KEP1_SEND, length, session->targetID, session->kep.sequenceNb);

	/* Put data in */
	memcpy(session->kep.cachedMessage + index, session->kep.generatedPointXY, 2 * SIZE * sizeof(word));

	return 1;
}

/**
 * 0: go to KEP_idle
 * 1: go to KEP1_wait
 */
int8_t KEP1_send_handlerBaseStation(struct SessionInfo* session) {
	if (session->kep.numTransmissions >= KEP_MAX_RETRANSMISSIONS) {
		/* Abort KEP */
		printf("BS\t- Max retransmissions reached on %x\n", session->kep.cachedMessage[0]);
		session->state.systemState = ClearSession;
		return 0;
	}

	/* Send message */
	transmit(&session->IO, session->kep.cachedMessage, KEP1_MESSAGE_BYTES, 1);

	/* Manage administration */
	session->kep.numTransmissions++;
	session->kep.timeOfTransmission = getMicrotime();

	return 1;
}

/**
 * -1: go to KEP(x)_send
 *  0: stay in KEP(x)_wait
 *  1: go to KEP(x)_verify
 */
int8_t KEP_wait_handlerBaseStation(struct SessionInfo* session, uint8_t expectedType) {
	uint64_t currentTime;
	uint64_t elapsedTime;

	/* Retransmit anyway if message is repeated. */
	if (session->receivedMessage.messageStatus == Message_repeated) {
		session->receivedMessage.messageStatus = Message_used;
		return -1;
	}

	/* If message is valid, first check type. */
	if (session->receivedMessage.messageStatus == Message_valid) {
		if (*session->receivedMessage.type == expectedType) {
			return 1;
		} else {
			session->receivedMessage.messageStatus = Message_used;
			return -1;
		}
	}

	currentTime = getMicrotime();
	elapsedTime = (currentTime - session->kep.timeOfTransmission) / MIC_PER_MIL;
	if (elapsedTime > KEP_RETRANSMISSION_TIMEOUT) {
		return -1;
	} else
		return 0;
}

/**
 * 0: go to KEP1_send
 * 1: go to KEP3_compute
 */
int8_t KEP3_verify_handlerBaseStation(struct SessionInfo* session) {
	word	XYout[2 * SIZE];
	word   *recvX, *recvY;
	uint8_t	signedMessage[4 * SIZE * sizeof(word)];
	uint8_t	correct;

	/* This function will use the received message. */
	session->receivedMessage.messageStatus = Message_used;

	/* Scalar multiplication */
	recvX = session->kep.receivedPointXY;
	recvY = session->kep.receivedPointXY + SIZE;
	memcpy(recvX, session->receivedMessage.curvePoint, SIZE * sizeof(word));
	memcpy(recvY, session->receivedMessage.curvePoint + SIZE, SIZE * sizeof(word));
	ECDHPointMultiply(session->kep.scalar, recvX, recvY, XYout, XYout + SIZE);

	/* Compute session key */
	sha3_HashBuffer(256, SHA3_FLAGS_NONE, XYout, 2 * SIZE * sizeof(word), session->sessionKey, AEGIS_KEY_NB);

	/* Decrypt and verify MAC + create AEGIS ctx for first time */
	init_AEGIS_ctx_IV(&session->aegisCtx, session->sessionKey, session->receivedMessage.IV);
	correct = aegisDecryptMessage(&session->aegisCtx, session->receivedMessage.message, 
								  session->receivedMessage.data - session->receivedMessage.type, FIELD_SIGN_NB);
	if (!correct)
		return 0;

	/* Verify signature */
	memcpy(signedMessage, session->receivedMessage.curvePoint, 2 * SIZE * sizeof(word));
	memcpy(signedMessage + 2 * SIZE * sizeof(word), session->kep.generatedPointXY, 2 * SIZE * sizeof(word));
	correct = ecdsaCheck(signedMessage, 4 * SIZE * sizeof(word), pkxDrone, pkyDrone,
						 session->receivedMessage.data, session->receivedMessage.data + SIZE * sizeof(word));

	/* Manage administration */
	if (correct) {
		session->kep.kepActive = 1;
		session->kep.numTransmissions 	= 0;
		session->kep.timeOfTransmission = 0;

		session->kep.expectedSequenceNb = addMultSeqNb(session->receivedMessage.seqNbNum, 1);
	}
	
	return correct;
}

/**
 * 0: stay in KEP3_compute
 * 1: go to KEP3_send
 */
int8_t KEP3_compute_handlerBaseStation(struct SessionInfo* session) {
	size_t		index;
	uint32_t	length;
	uint8_t		IV[AEGIS_IV_NB];
	word 		messageToSign[4 * SIZE];

	memcpy(messageToSign, session->kep.generatedPointXY, 2 * SIZE * sizeof(word));
	memcpy(messageToSign + 2 * SIZE, session->kep.receivedPointXY, 2 * SIZE * sizeof(word));
	ecdsaSign(messageToSign, 4 * SIZE * sizeof(word), privBS, session->kep.signature, session->kep.signature + SIZE);

	length = KEP3_MESSAGE_BYTES;
	getRandomBytes(AEGIS_IV_NB, IV);
	addOneSeqNb(&session->kep.sequenceNb);
	index = encodeMessage(session->kep.cachedMessage, TYPE_KEP3_SEND, length, session->targetID, session->kep.sequenceNb, IV);

	/* Put data in */
	memcpy(session->kep.cachedMessage + index, session->kep.signature, 2 * SIZE * sizeof(word));

	/* Encrypt and MAC */
	session->aegisCtx.iv = IV;
	aegisEncryptMessage(&session->aegisCtx, session->kep.cachedMessage, index, FIELD_SIGN_NB);

	return 1;
}

/**
 * 0: go to idle
 * 1: go to KEP3_wait
 */
int8_t KEP3_send_handlerBaseStation(struct SessionInfo* session) {
	if (session->kep.numTransmissions >= KEP_MAX_RETRANSMISSIONS) {
		/* Abort KEP */
		printf("BS\t- Max retransmissions reached on %x\n", session->kep.cachedMessage[0]);
		session->state.systemState = ClearSession;
		return 0;
	}

	/* Send message */
	transmit(&session->IO, session->kep.cachedMessage, KEP3_MESSAGE_BYTES, 1);

	/* Manage administration */
	session->kep.numTransmissions++;
	session->kep.timeOfTransmission = getMicrotime();

	return 1;
}

/**
 * 0: go to KEP3_send
 * 1: go to Done
 */
int8_t KEP5_verify_handlerBaseStation(struct SessionInfo* session) {
	uint8_t correct;

	/* This function will use the received message. */
	session->receivedMessage.messageStatus = Message_used;

	/* Verify MAC */
	session->aegisCtx.iv = session->receivedMessage.IV;
	correct = aegisDecryptMessage(&session->aegisCtx, session->receivedMessage.message,
								  session->receivedMessage.MAC - session->receivedMessage.type, 0);
	if (!correct)
		return 0;

	if (session->kep.sequenceNb == session->receivedMessage.ackSeqNbNum) {
		session->kep.expectedSequenceNb = addMultSeqNb(session->receivedMessage.seqNbNum, 1);
		return 1;
	} else {
		return 0;
	}
}

/* Public functions */

void init_KEP_ctxBaseStation(struct KEP_ctx* ctx) {
	ctx->kepActive = 0;
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

kepState kepContinueBaseStation(struct SessionInfo* session, kepState currentState) {
	switch (currentState) {
	case KEP_idle:
		return KEP1_idle_handlerBaseStation(session) ? KEP1_compute : KEP_idle;

	case KEP1_compute:
		return KEP1_compute_handlerBaseStation(session) ? KEP1_send : KEP1_compute;

	case KEP1_send:
		return KEP1_send_handlerBaseStation(session) ? KEP1_wait : KEP_idle;

	case KEP1_wait:
		switch (KEP_wait_handlerBaseStation(session, TYPE_KEP2_SEND)) {
		case -1: return KEP1_send;
		case 0:  return KEP1_wait;
		case 1:  return KEP3_verify;
		default: return KEP_idle;
		}

	case KEP3_verify:
		return KEP3_verify_handlerBaseStation(session) ? KEP3_compute : KEP1_send;

	case KEP3_compute:
		return KEP3_compute_handlerBaseStation(session) ? KEP3_send : KEP3_compute;

	case KEP3_send:
		return KEP3_send_handlerBaseStation(session) ? KEP3_wait : KEP_idle;

	case KEP3_wait:
		switch (KEP_wait_handlerBaseStation(session, TYPE_KEP4_SEND)) {
		case -1: return KEP3_send;
		case 0:  return KEP3_wait;
		case 1:  return KEP5_verify;
		default: return KEP_idle;
		}

	case KEP5_verify:
		return KEP5_verify_handlerBaseStation(session) ? Done : KEP3_send;

	default:
		return KEP_idle;
	}
}