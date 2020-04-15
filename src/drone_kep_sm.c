#include "./../include/drone_kep_sm.h"

signed_word KEP2_precompute_handlerDrone(struct SessionInfo* session) {
	ECDHGenerateRandomSample(session->kep.scalar, session->kep.generatedPointXY, session->kep.generatedPointXY + SIZE);
	return 0;
}

signed_word KEP2_wait_request_handlerDrone(struct SessionInfo* session) {
	return session->receivedMessage.type != TYPE_KEP1_SEND;
}

signed_word KEP2_compute_handlerDrone(struct SessionInfo* session) {
	word XYout[2 * SIZE];
	uint8_t concatPoints[4 * SIZE * sizeof(word)];
	uint8_t	IV[AEGIS_IV_NB];

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
	return 0;
}

signed_word KEP2_send_handlerDrone(struct SessionInfo* session) {
	word index;
	uint32_t length;
	uint8_t IV[AEGIS_IV_NB];

	if (session->kep.numTransmissions >= KEP_MAX_RETRANSMISSIONS) {
		/* Abort KEP. */
		session->state.systemState = ClearSession;
		return 1;
	}

	if (!session->kep.cachedMessageValid) {
		length = KEP2_MESSAGE_BYTES;
		getRandomBytes(AEGIS_IV_NB, IV);
		index = encodeMessage(session->kep.cachedMessage, TYPE_KEP2_SEND, &length, session->targetID, session->sequenceNb, IV);

		/* Put data in */
		memcpy(session->kep.cachedMessage + index, session->kep.generatedPointXY, 2 * SIZE * sizeof(word));
		memcpy(session->kep.cachedMessage + index + FIELD_CURVEPOINT_NB, session->kep.signature, 2 * SIZE * sizeof(word));

		/* Encrypt */
		setIV(&session->aegisCtx, IV);
		aegisEncryptMessage(&session->aegisCtx, session->kep.cachedMessage, index + FIELD_CURVEPOINT_NB, FIELD_SIGN_NB);
	}

	/* Send message */
	while (transmit(&session->IO, session->kep.cachedMessage, KEP2_MESSAGE_BYTES, 1) == -1);

	/* Manage administration */
	addOneSeqNb(&session->sequenceNb);
	session->kep.numTransmissions++;
	session->kep.timeOfTransmission = clock();

	return 0;
}

signed_word KEP2_wait_handlerDrone(struct SessionInfo* session) {
	double_word  currentTime;
	double_word  elapsedTime;

	currentTime = (double_word)clock();
	elapsedTime = ((float_word)currentTime - session->kep.timeOfTransmission) / CLOCKS_PER_SEC;
	if (elapsedTime > KEP_RETRANSMISSION_TIMEOUT)
		return -1;
	
	return session->receivedMessage.type == TYPE_KEP2_SEND;
}

signed_word KEP4_verify_handlerDrone(struct SessionInfo* session) {
	uint8_t correct;
	uint8_t messageToSign[4 * SIZE * sizeof(word)];

	/* MAC and Decryption -> see poll and decode */

	memcpy(messageToSign, session->kep.receivedPointXY, 2 * SIZE * sizeof(word));
	memcpy(messageToSign + 2 * SIZE * sizeof(word), session->kep.generatedPointXY, 2 * SIZE * sizeof(word));

	correct = ecdsaCheck(messageToSign, 4 * SIZE * sizeof(word), pkxBS, pkyBS,
		session->receivedMessage.data, session->receivedMessage.data + SIZE * sizeof(word));

	if (correct) {
		session->kep.cachedMessageValid = 0;
		session->kep.numTransmissions = 0;
		session->kep.timeOfTransmission = 0;
	}

	return !correct;
}

signed_word KEP4_send_handlerDrone(struct SessionInfo* session) {
	word index;
	uint32_t length;
	uint8_t IV[AEGIS_IV_NB];

	if (session->kep.numTransmissions >= KEP_MAX_RETRANSMISSIONS) {
		/* Abort KEP. */
		session->state.systemState = ClearSession;
		return 1;
	}

	if (!session->kep.cachedMessageValid) {
		length = KEP4_MESSAGE_BYTES;
		getRandomBytes(AEGIS_IV_NB, IV);
		index = encodeMessage(session->kep.cachedMessage, TYPE_KEP4_SEND, &length, session->targetID, session->sequenceNb, IV);

		/* Put data in */
		memcpy(session->kep.cachedMessage + index, session->receivedMessage.seqNb, FIELD_SEQNB_NB);
		
		/* Encrypt */
		setIV(&session->aegisCtx, IV);
		aegisEncryptMessage(&session->aegisCtx, session->kep.cachedMessage, index + FIELD_SEQNB_NB, 0);
	}

	/* Send message */
	while (transmit(&session->IO, session->kep.cachedMessage, KEP4_MESSAGE_BYTES, 1) == -1);

	/* Manage administration */
	addOneSeqNb(&session->sequenceNb);
	session->kep.numTransmissions++;
	session->kep.timeOfTransmission = clock();

	return 0;
}

signed_word KEP4_wait_handler(struct SessionInfo* session) {
	double_word  currentTime;
	double_word  elapsedTime;

	currentTime = (double_word)clock();
	elapsedTime = ((float_word)currentTime - session->kep.timeOfTransmission) / CLOCKS_PER_SEC;
	if (elapsedTime > 2 * KEP_RETRANSMISSION_TIMEOUT)
		return 1;

	if (session->receivedMessage.type == TYPE_KEP3_SEND)
		return -1;
	else
		return 0;
}

/* Public functions */

void init_KEP_ctxDrone(struct KEP_ctx* ctx) {
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

kepState kepContinueDrone(struct SessionInfo* session, kepState currentState) {
	switch (currentState) {
	case KEP2_precompute:
		if (!KEP2_precompute_handlerDrone(session))
            return KEP2_wait;
        else
            return KEP2_precompute;

	case KEP2_wait_request:
		if (!KEP2_wait_request_handlerDrone(session))
			return KEP2_compute;
		else
			return KEP2_wait;

	case KEP2_compute:
		if (!KEP2_compute_handlerDrone(session))
			return KEP2_send;
		else
			return KEP2_compute;

	case KEP2_send:
		if (!KEP2_send_handlerDrone(session))
			return KEP2_wait;
		else
			return KEP_idle; /* Handler sets systemstate to clear session */

	case KEP2_wait:
		switch (KEP2_wait_handlerDrone(session)) {
		case -1: return KEP2_send;
		case 0:  return KEP2_wait;
		case 1:  return KEP4_verify;
		}

	case KEP4_verify:
		if (!KEP4_verify_handlerDrone(session))
			return KEP4_send;
		else
			return KEP2_send;

	case KEP4_send:
		if (!KEP4_send_handlerDrone(session))
			return KEP4_wait;
		else
			return KEP_idle; /* Handler sets systemstate to clear session */

	case KEP4_wait:
		switch (KEP4_wait_handler(session)) {
		case -1: return KEP4_send;
		case 0:  return KEP4_wait;
		case 1:  return Done;
		}

	default:
		return KEP_idle;
	}
}