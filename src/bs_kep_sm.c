#include "./../include/bs_kep_sm.h"

/** 
 * Helper functions
 */
void formMessageArray(uint8_t* message, uint8_t type, word length, word targetID, word seqNb, word hasIV) {
	word ivOffset;
	word i;

	message[0] = type;
	message[1] = length;

	ivOffset = hasIV ? 16 : 0;
	message[4 + ivOffset] = targetID;
	
#if (AIR_LITTLE_ENDIAN && PROC_LITTLE_ENDIAN) || (AIR_BIG_ENDIAN && PROC_BIG_ENDIAN)
	memcpy(message[12 + ivOffset], &seqNb, sizeof(word));
#else
	for (i = 0; i < sizeof(word); i++)
		message[12 + ivOffset + i] = ((uint8_t*)&seqNb)[sizeof(word) - i]; // TODO: check dit!
#endif
}

/**
 * State handlers
 */
word KEP1_compute_handler(struct SessionInfo* session) {
    word X[SIZE], Y[SIZE], Z[SIZE];

    ECDHGenerateRandomSample(session->kep.scalar, X, Y, Z);
    toCartesian(X, Y, Z, session->kep.generatedPointX, session->kep.generatedPointY);
    return 0;
}

word KEP1_send_handler(struct SessionInfo* session) {
    uint8_t buffer[KEP1_MESSAGE_BYTES];

    if (session->kep.numTransmissions >= KEP_MAX_RETRANSMISSIONS) {
        /* Abort KEP */
        session->state.systemState = ClearSession;
        return 1;
    }

    /* Form message */
	formMessageArray(buffer, TYPE_KEP1_SEND, KEP1_MESSAGE_BYTES, session->targetID, session->sequenceNb, 0);
	/* PUT data in */

	return 0;
}

word KEP1_wait_handler(struct SessionInfo* session) {
    double_word  currentTime;
    double_word  elapsedTime;

    currentTime = (double_word)clock();
    elapsedTime = ((float_word)currentTime - session->kep.timeOfTransmission) / CLOCKS_PER_SEC;
	return 0;
}

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
	switch (currentState)
	{
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
			return KEP_idle; /* handler sets systemstate to clear session */

	default:
		return KEP_idle;
		break;
	}
}
