#include "./../include/drone_kep_sm.h"

/* State handlers return 0 if successful, non-zero else. */

signed_word KEP2_precompute_handlerDrone(struct SessionInfo* session) {
	word X[SIZE], Y[SIZE], Z[SIZE];

	ECDHGenerateRandomSample(session->kep.scalar, X, Y, Z);
	toCartesian(X, Y, Z, session->kep.generatedPointX, session->kep.generatedPointY);
	return 0;
}

signed_word KEP2_compute_handlerDrone(struct SessionInfo* session) {
    /* Compute signature and authenticated encryption on DH sample. */
}

signed_word KEP2_send_handlerDrone(struct SessionInfo* session) {
	uint8_t buffer[KEP2_MESSAGE_BYTES];

	if (session->kep.numTransmissions >= KEP_MAX_RETRANSMISSIONS) {
		/* Abort KEP */
		session->state.systemState = ClearSession;
		return 1;
	}

	/* Form message */
	/* formMessageArray(buffer, TYPE_KEP1_SEND, KEP1_MESSAGE_BYTES, session->targetID, session->sequenceNb, 0); */

	/* PUT data in */
    /* Still wrong. */
	memcpy(buffer, session->kep.generatedPointX, SIZE * sizeof(word));
	memcpy(buffer, session->kep.generatedPointY, SIZE * sizeof(word));

	/* Send message */
	while (transmit(&session->IO, buffer, KEP2_MESSAGE_BYTES, 1) == -1);

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
	memset(ctx->scalar, 0, sizeof(word) * SIZE);
	memset(ctx->generatedPointX, 0, sizeof(word) * SIZE);
	memset(ctx->generatedPointY, 0, sizeof(word) * SIZE);
	memset(ctx->receivedPointX, 0, sizeof(word) * SIZE);
	memset(ctx->receivedPointY, 0, sizeof(word) * SIZE);
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