#include "./../include/bs_kep_sm.h"

/* Small helper */
inline void addOneSeqNb(uint8_t* seqNb) {
	word i, iszero;

	iszero = 1;
	for (i = 0; i < FIELD_SEQNB_NB; i++) {
		if (seqNb[i] == 0xff)
			seqNb[i] = 0x00;
		else {
			seqNb[i]++;
			iszero = 0;
		}
	}
	if (iszero)
		seqNb[0] = 0x01;
}

/* State handlers return 0 if successful, non-zero else. */
word KEP1_compute_handlerBaseStation(struct SessionInfo* session) {
	word X[SIZE], Y[SIZE], Z[SIZE];

	ECDHGenerateRandomSample(session->kep.scalar, X, Y, Z);
	toCartesian(X, Y, Z, session->kep.generatedPointX, session->kep.generatedPointY);
	return 0;
}

word KEP1_send_handlerBaseStation(struct SessionInfo* session) {
	uint8_t message[KEP1_MESSAGE_BYTES];
	word index;

	if (session->kep.numTransmissions >= KEP_MAX_RETRANSMISSIONS) {
		/* Abort KEP */
		session->state.systemState = ClearSession;
		return 1;
	}

	/* Form message */
	index = encodeMessageNoEncryption(message, TYPE_KEP1_SEND, KEP1_MESSAGE_BYTES, session->targetID, session->sequenceNb);

	/* PUT data in */
	memcpy(message + index, session->kep.generatedPointX, SIZE * sizeof(word));
	memcpy(message + index + SIZE * sizeof(word), session->kep.generatedPointY, SIZE * sizeof(word));

	/* Send message */
	while (transmit(&session->IO, message, KEP1_MESSAGE_BYTES, 1) == -1);

	/* Manage administration */
	session->kep.numTransmissions++;

	return 0;
}

word KEP1_wait_handlerBaseStation(struct SessionInfo* session) {
	double_word  currentTime;
	double_word  elapsedTime;

	currentTime = (double_word)clock();
	elapsedTime = ((float_word)currentTime - session->kep.timeOfTransmission) / CLOCKS_PER_SEC;
	if (elapsedTime > KEP_RETRANSMISSION_TIMEOUT)
		return 1;

	return 0;
}

word KEP3_verify_handlerBaseState(struct SessionInfo* session) {
	word xyZ[3 * SIZE]; /* To avoid guard space between variables -> easier input sha3 */
	uint8_t* x, y;
	uint8_t ad[FIELD_TYPE_NB + FIELD_LENGTH_NB + AEGIS_IV_NB + FIELD_TARGET_NB + FIELD_SEQNB_NB + FIELD_KEP2_SIGN_OF];
	uint8_t expectedMAC[AEGIS_MAC_NB];

	/* Scalar multiplication */
	x = session->receivedMessage.data + FIELD_KEP2_BGX_OF;
	y = session->receivedMessage.data + FIELD_KEP2_BGY_OF;
	toJacobian(x, y, x, y, xyZ + 2 * SIZE);
	pointMultiply(session->kep.scalar, x, y, one_mont, xyZ, xyZ + SIZE, xyZ + 2 * SIZE);
	toCartesian(xyZ, xyZ + SIZE, xyZ + 2 * SIZE, xyZ, xyZ + SIZE);

	/* Compute session key */
	sha3_HashBuffer(256, SHA3_FLAGS_NONE, xyZ, 2 * SIZE * sizeof(word), session->sessionKey, 16);

	/* Decrypt and verify MAC + create AEGIS ctx for first time */
	init_AEGIS_ctx_IV(&session->aegisCtx, session->sessionKey, session->receivedMessage.IV);
	/* Only place where we should use aegisDecrypt instead of aegisDecryptMessage */
	memcpy(ad, session->receivedMessage.type, FIELD_TYPE_NB);
	memcpy(ad + FIELD_TYPE_NB, session->receivedMessage.length, FIELD_LENGTH_NB);
	memcpy(ad + FIELD_TYPE_NB + FIELD_LENGTH_NB, session->receivedMessage.IV, AEGIS_IV_NB);
	memcpy(ad + FIELD_TYPE_NB + FIELD_LENGTH_NB + AEGIS_IV_NB, session->receivedMessage.targetID, FIELD_TARGET_NB);
	memcpy(ad + FIELD_TYPE_NB + FIELD_LENGTH_NB + AEGIS_IV_NB + FIELD_TARGET_NB, session->receivedMessage.seqNb, FIELD_SEQNB_NB);
	memcpy(ad + FIELD_TYPE_NB + FIELD_LENGTH_NB + AEGIS_IV_NB + FIELD_TARGET_NB + FIELD_SEQNB_NB, session->receivedMessage.data, FIELD_KEP2_SIGN_OF);
	aegisDecrypt(&session->aegisCtx, ad, sizeof(ad), /* sizeof should work */
		session->receivedMessage.data + FIELD_KEP2_SIGN_OF, FIELD_KEP2_SIGN_NB,
		session->receivedMessage.data + FIELD_KEP2_SIGN_OF, expectedMAC);
	if (!equalByteArrays(expectedMAC, session->receivedMessage.data + FIELD_KEP2_SIGN_OF + FIELD_KEP2_SIGN_NB, AEGIS_MAC_NB))
		return 1;

	/* Verify signature */
}

/* Public functions */

void init_KEP_ctxBaseStation(struct KEP_ctx* ctx) {
	ctx->timeOfTransmission = clock();
	ctx->numTransmissions = 0;

	/* Set arrays to zero */
	memset(ctx->scalar, 0, sizeof(word) * SIZE);
	memset(ctx->generatedPointX, 0, sizeof(word) * SIZE);
	memset(ctx->generatedPointY, 0, sizeof(word) * SIZE);
	memset(ctx->receivedPointX, 0, sizeof(word) * SIZE);
	memset(ctx->receivedPointY, 0, sizeof(word) * SIZE);
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

	default:
		return KEP_idle;
	}
}


/* Q: can point multiply result be equal to operands? -> NEE */
/* Q: Ok to calculate session key on jacobian point? -> NEE */

/* TODO: read length in decoder doesnt work as it is now an array.... */
/* TODO: assign long term keys */