#include "./../include/bs_kep_sm.h"

/* State handlers return 0 if successful, non-zero else. */
word KEP1_compute_handlerBaseStation(struct SessionInfo* session) {
	ECDHGenerateRandomSample(session->kep.scalar, session->kep.generatedPointXY, session->kep.generatedPointXY + SIZE);
	return 0;
}

signed_word KEP1_send_handlerBaseStation(struct SessionInfo* session) {
	word index;
	uint32_t length;

	if (!session->kep.cachedMessageValid) {

		if (session->kep.numTransmissions >= KEP_MAX_RETRANSMISSIONS) {
			/* Abort KEP */
			session->state.systemState = ClearSession;
			return 1;
		}

		/* Form message */
		length = KEP1_MESSAGE_BYTES;
		index = encodeMessageNoEncryption(session->kep.cachedMessage, TYPE_KEP1_SEND, &length, session->targetID, &session->sequenceNb);

		/* Put data in */
		memcpy(session->kep.cachedMessage + index, session->kep.generatedPointXY, 2 * SIZE * sizeof(word));

		session->kep.cachedMessageValid = 1;
	}

	/* Send message */
	while (transmit(&session->IO, session->kep.cachedMessage, KEP1_MESSAGE_BYTES, 1) == -1);

	/* Manage administration */
	session->kep.numTransmissions++;
	session->kep.timeOfTransmission = clock();
	addOneSeqNb(&session->sequenceNb);

	return 0;
}

signed_word KEP1_wait_handlerBaseStation(struct SessionInfo* session) {
	double_word  currentTime;
	double_word  elapsedTime;

	currentTime = (double_word)clock();
	elapsedTime = ((float_word)currentTime - session->kep.timeOfTransmission) / CLOCKS_PER_SEC;
	if (elapsedTime > KEP_RETRANSMISSION_TIMEOUT)
		return -1;

	return session->receivedMessage.type == TYPE_KEP2_SEND;
}

signed_word KEP3_verify_handlerBaseStation(struct SessionInfo* session) {
	word	XYout[2 * SIZE];
	word*	recvX, recvY;
	uint8_t	signedMessage[4 * SIZE * sizeof(word)];
	word	correct;

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
		session->receivedMessage.data - session->receivedMessage.type, FIELD_SIGN_NB); // IS SUB CIRRECT?
	if (!correct)
		return 1;

	/* Verify signature */
	memcpy(signedMessage, session->receivedMessage.curvePoint, 2 * SIZE * sizeof(word));
	memcpy(signedMessage + 2 * SIZE * sizeof(word), session->kep.generatedPointXY, 2 * SIZE * sizeof(word));
	correct = ecdsaCheck(signedMessage, 4 * SIZE * sizeof(word), pkxDrone, pkyDrone,
		session->receivedMessage.data, session->receivedMessage.data + SIZE * sizeof(word));

	/* Manage administration */
	if (correct) {
		session->kep.cachedMessageValid = 0;
		session->kep.numTransmissions = 0;
		session->kep.timeOfTransmission = 0;
	}

	return !correct;
}

signed_word KEP3_compute_handlerBaseStation(struct SessionInfo* session) {
	word	messageToSign[4 * SIZE * sizeof(word)];

	memcpy(messageToSign, session->kep.generatedPointXY, 2 * SIZE * sizeof(word));
	memcpy(messageToSign + 2 * SIZE, session->kep.receivedPointXY, 2 * SIZE * sizeof(word));
	ecdsaSign(messageToSign, 4 * SIZE * sizeof(word), privBS, session->kep.signature, session->kep.signature + SIZE);

	return 0;
}

signed_word KEP3_send_handlerBaseStation(struct SessionInfo* session) {
	word		index;
	uint32_t	length;
	uint8_t		IV[AEGIS_IV_NB];

	if (session->kep.numTransmissions >= KEP_MAX_RETRANSMISSIONS) {
		/* Abort KEP */
		session->state.systemState = ClearSession;
		return 1;
	}

	if (!session->kep.cachedMessageValid) {
		length = KEP3_MESSAGE_BYTES;
		getRandomBytes(AEGIS_IV_NB, IV);
		index = encodeMessage(session->kep.cachedMessage, TYPE_KEP3_SEND, length, session->targetID, session->sequenceNb, IV);

		/* Put data in */
		memcpy(session->kep.cachedMessage + index, session->kep.signature, 2 * SIZE * sizeof(word));

		/* Encrypt and MAC */
		setIV(&session->aegisCtx, IV);
		aegisEncryptMessage(&session->aegisCtx, session->kep.cachedMessage, index, FIELD_SIGN_NB);

		session->kep.cachedMessageValid = 1;
	}

	/* Send message */
	while (transmit(&session->IO, session->kep.cachedMessage, KEP3_MESSAGE_BYTES, 1) == -1);

	/* Manage administration */
	session->kep.numTransmissions++;
	session->kep.timeOfTransmission = clock();
	addOneSeqNb(&session->sequenceNb);

	return 0;
}

signed_word KEP3_wait_handlerBaseStation(struct SessionInfo* session) {
	double_word  currentTime;
	double_word  elapsedTime;

	currentTime = (double_word)clock();
	elapsedTime = ((float_word)currentTime - session->kep.timeOfTransmission) / CLOCKS_PER_SEC;
	if (elapsedTime > KEP_RETRANSMISSION_TIMEOUT)
		return -1;

	return session->receivedMessage.type == TYPE_KEP3_SEND;
}

signed_word KEP5_verify_handlerBaseStation(struct SessionInfo* session) {
	/* If we got here, MAC is ok */
	return session->sequenceNb != session->receivedMessage.ackSeqNbNum;
}

/* Public functions */

void init_KEP_ctxBaseStation(struct KEP_ctx* ctx) {
	ctx->timeOfTransmission = 0;
	ctx->numTransmissions = 0;
	ctx->cachedMessageValid = 0;

	/* Set arrays to zero */
	memset(ctx->scalar, 0, sizeof(word) * SIZE);
	memset(ctx->generatedPointXY, 0, 2 * sizeof(word) * SIZE);
	memset(ctx->receivedPointXY, 0, 2 * sizeof(word) * SIZE);
	memset(ctx->signature, 0, 2 * sizeof(word) * SIZE);
	memset(ctx->cachedMessage, 0, KEP2_MESSAGE_BYTES);
}

/* Idea: instead of returning the new state, we could maybe update it in
   session->state.kepState since we have session available. This reduces
   the work for the caller. */
kepState kepContinueBaseStation(struct SessionInfo* session, kepState currentState) {
	switch (currentState) {
	case KEP_idle:
		return KEP1_compute;

	case KEP1_compute:
		if (!KEP1_compute_handlerBaseStation(session))
			return KEP1_send;
		else
			return KEP1_compute;

	case KEP1_send:
		if (!KEP1_send_handlerBaseStation(session))
			return KEP1_wait;
		else
			return KEP_idle; /* Handler sets systemstate to clear session */

	case KEP1_wait:
		switch (KEP1_wait_handlerBaseStation(session)) {
		case -1: return KEP1_send;
		case 0:  return KEP1_wait;
		case 1:  return KEP3_verify;
		}

	case KEP3_verify:
		if (!KEP3_verify_handlerBaseStation(session))
			return KEP3_compute;
		else
			return KEP1_send;

	case KEP3_compute:
		if (!KEP3_compute_handlerBaseStation(session))
			return KEP3_send;
		else
			return KEP3_compute;

	case KEP3_send:
		if (!KEP3_send_handlerBaseStation(session))
			return KEP3_wait;
		else
			return KEP_idle; /* Handler sets systemstate to clear session */

	case KEP3_wait:
		switch (KEP3_wait_handlerBaseStation(session)) {
		case -1: return KEP3_send;
		case 0:  return KEP3_wait;
		case 1:  return KEP5_verify;
		}

	case KEP5_verify:
		if (!KEP5_verify_handlerBaseStation(session))
			return Done;
		else
			return KEP3_send;

	default:
		return KEP_idle;
	}
}