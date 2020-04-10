#include "./../include/drone_kep_sm.h"

/* State handlers return 0 if successful, non-zero else. */

signed_word KEP2_precompute_handlerDrone(struct SessionInfo* session) {
	ECDHGenerateRandomSample(session->kep.scalar, session->kep.generatedPointXY, session->kep.generatedPointXY + SIZE);
	return 0;
}

signed_word KEP2_compute_handlerDrone(struct SessionInfo* session) {
	word X[SIZE], Y[SIZE], Z[SIZE];
	word XYres[2 * SIZE];
	word concatPoints[4 * SIZE];
	word r[SIZE], s[SIZE];
	uint8_t	IV[AEGIS_IV_NB];
	uint32_t length = KEP2_MESSAGE_BYTES;

	/* Copy received point to state. */
	memcpy(session->kep.receivedPointXY, session->receivedMessage.data + FIELD_KEP1_AGX_OF, SIZE * sizeof(word));
	memcpy(session->kep.receivedPointXY + SIZE, session->receivedMessage.data + FIELD_KEP1_AGY_OF, SIZE * sizeof(word));

	/* Compute resulting point on elliptic curve. */
	ECDHPointMultiply(session->kep.scalar, session->kep.receivedPointXY, session->kep.receivedPointXY + SIZE, XYres, XYres + SIZE);

	/* Compute session key. */
	sha3_HashBuffer(256, SHA3_FLAGS_NONE, XYres, 2 * SIZE * sizeof(word), session->sessionKey, 16);

	/* Compute signature. */
	memcpy(concatPoints, session->kep.generatedPointXY, 2 * SIZE * sizeof(word));
	memcpy(concatPoints + 2 * SIZE, session->kep.receivedPointXY, 2 * SIZE);
	ecdsaSign(concatPoints, 4 * SIZE * sizeof(word), privDrone, r, s);

	/* Construct message. */
	getRandomBytes(AEGIS_IV_NB, IV);
	encodeMessage(session->kep.cachedMessage, TYPE_KEP2_SEND, length, session->targetID, session->sequenceNb, IV);

	return 0;
}

signed_word KEP2_send_handlerDrone(struct SessionInfo* session) {
	if (session->kep.numTransmissions >= KEP_MAX_RETRANSMISSIONS) {
		/* Abort KEP. */
		session->state.systemState = ClearSession;
		/*session->state.kepState = */
		return 1;
	}

	/* Form message */
	/* formMessageArray(buffer, TYPE_KEP1_SEND, KEP1_MESSAGE_BYTES, session->targetID, session->sequenceNb, 0); */

	/* PUT data in */
    /* Still wrong. */
	/*memcpy(buffer, session->kep.generatedPointXY, SIZE * sizeof(word));*/
	/*memcpy(buffer, session->kep.generatedPointXY + SIZE, SIZE * sizeof(word));*/

	/* Send message */
	/*while (transmit(&session->IO, buffer, KEP2_MESSAGE_BYTES, 1) == -1);*/

	return 0;
}

signed_word KEP2_wait_handlerDrone(struct SessionInfo* session) {
	double_word  currentTime;
	double_word  elapsedTime;

	currentTime = (double_word)clock();
	elapsedTime = ((float_word)currentTime - session->kep.timeOfTransmission) / CLOCKS_PER_SEC;
	if (elapsedTime > KEP_RETRANSMISSION_TIMEOUT)
		return -1;
	
	return 0;
}

/* Public functions */

void init_KEP_ctxDrone(struct KEP_ctx* ctx) {
	ctx->timeOfTransmission = clock();
	ctx->numTransmissions = 0;

	/* Set arrays to zero */
	/*memset(ctx->scalar, 0, sizeof(word) * SIZE);
	memset(ctx->generatedPointX, 0, sizeof(word) * SIZE);
	memset(ctx->generatedPointY, 0, sizeof(word) * SIZE);
	memset(ctx->receivedPointX, 0, sizeof(word) * SIZE);
	memset(ctx->receivedPointY, 0, sizeof(word) * SIZE);*/
}

kepState kepContinueDrone(struct SessionInfo* session, kepState currentState) {
	switch (currentState) {
	case KEP2_precompute:
		if (!KEP2_precompute_handlerDrone(session))
            return KEP2_compute;
        else
            return KEP2_precompute;

	case KEP2_compute:
		if (!KEP2_compute_handlerDrone(session))
			return KEP1_send;
		else
			return KEP1_compute;

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

	default:
		return KEP_idle;
	}
}