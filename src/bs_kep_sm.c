#include "./../include/bs_kep_sm.h"

/**
 * State handlers
 */
signed_word KEP1_compute_handler(struct SessionInfo* session) {
	word X[SIZE], Y[SIZE], Z[SIZE];

	ECDHGenerateRandomSample(session->kep.scalar, X, Y, Z);
	toCartesian(X, Y, Z, session->kep.generatedPointX, session->kep.generatedPointY);
	return 0;
}

signed_word KEP1_send_handler(struct SessionInfo* session) {
	uint8_t buffer[KEP1_MESSAGE_BYTES];

	if (session->kep.numTransmissions >= KEP_MAX_RETRANSMISSIONS) {
		/* Abort KEP */
		session->state.systemState = ClearSession;
		return 1;
	}

	/* Form message */
	/* formMessageArray(buffer, TYPE_KEP1_SEND, KEP1_MESSAGE_BYTES, session->targetID, session->sequenceNb, 0); */

	/* PUT data in */
	memcpy(buffer, session->kep.generatedPointX, SIZE * sizeof(word));
	memcpy(buffer, session->kep.generatedPointY, SIZE * sizeof(word));

	/* Send message */
	while (transmit(&session->IO, buffer, KEP1_MESSAGE_BYTES, 1) == -1);

	return 0;
}

signed_word KEP1_wait_handler(struct SessionInfo* session) {
	double_word  currentTime;
	double_word  elapsedTime;

	currentTime = (double_word)clock();
	elapsedTime = ((float_word)currentTime - session->kep.timeOfTransmission) / CLOCKS_PER_SEC;
	if (elapsedTime > KEP_RETRANSMISSION_TIMEOUT)
		return -1;
	
	return 0;
}

/** 
 * Public functions
 */

void init_KEP_ctx(struct KEP_ctx* ctx) {
	ctx->timeOfTransmission = clock();
	ctx->numTransmissions = 0;

	/* Set arrays to zero */
	memset(ctx->scalar, 0, sizeof(word) * SIZE);
	memset(ctx->generatedPointX, 0, sizeof(word) * SIZE);
	memset(ctx->generatedPointY, 0, sizeof(word) * SIZE);
	memset(ctx->receivedPointX, 0, sizeof(word) * SIZE);
	memset(ctx->receivedPointY, 0, sizeof(word) * SIZE);
}

kepState kepContinue(struct SessionInfo* session, kepState currentState) {
	switch (currentState) {
	case KEP_idle:
		return KEP1_compute;

	case KEP1_compute:
		if (!KEP1_compute_handler(session))
			return KEP1_send;
		else
			return KEP1_compute;

	case KEP1_send:
		if (!KEP1_send_handler(session))
			return KEP1_wait;
		else
			return KEP_idle; /* Handler sets systemstate to clear session */

	case KEP1_wait:
		switch (KEP1_wait_handler(session)) {
		case -1: return KEP1_send;
		case 0:  return KEP1_wait;
		case 1:  return KEP3_verify;
		}


	default:
		return KEP_idle;
		break;
	}
}