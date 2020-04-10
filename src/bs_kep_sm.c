#include "./../include/bs_kep_sm.h"

/* Small helper */
/*inline*/ void addOneSeqNb(uint8_t* seqNb) {
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
	ECDHGenerateRandomSample(session->kep.scalar, session->kep.generatedPointXY, session->kep.generatedPointXY + SIZE);
	return 0;
}

word KEP1_send_handlerBaseStation(struct SessionInfo* session) {
#if(ENDIAN_CONVERT) 
	signed_word i, j;
#endif
	word index;

	if (!session->kep.cachedMessageValid) {

		if (session->kep.numTransmissions >= KEP_MAX_RETRANSMISSIONS) {
			/* Abort KEP */
			session->state.systemState = ClearSession;
			return 1;
		}

		/* Form message */
		index = encodeMessageNoEncryption(session->kep.cachedMessage, TYPE_KEP1_SEND, KEP1_MESSAGE_BYTES, session->targetID, session->sequenceNb);

		/* Put data in */
		wordArrayToByteArray(session->kep.cachedMessage + index, session->kep.generatedPointXY, SIZE * sizeof(word));
		wordArrayToByteArray(session->kep.cachedMessage + index + SIZE * sizeof(word), session->kep.generatedPointXY + SIZE, SIZE * sizeof(word));

		session->kep.cachedMessageValid = 1;
	}

	/* Send message */
	while (transmit(&session->IO, session->kep.cachedMessage, KEP1_MESSAGE_BYTES, 1) == -1);

	/* Manage administration */
	session->kep.numTransmissions++;
	session->kep.timeOfTransmission = clock();

	return 0;
}

word KEP1_wait_handlerBaseStation(struct SessionInfo* session) {
	double_word  currentTime;
	double_word  elapsedTime;

	currentTime = (double_word)clock();
	elapsedTime = ((float_word)currentTime - session->kep.timeOfTransmission) / CLOCKS_PER_SEC;
	if (elapsedTime > KEP_RETRANSMISSION_TIMEOUT)
		return -1;

	return session->receivedMessage.type == TYPE_KEP2_SEND;
}

word KEP3_verify_handlerBaseStation(struct SessionInfo* session) {
#if(ENDIAN_CONVERT) 
	signed_word i, j;
	word	shaBuff[2 * SIZE];
	word	r[SIZE], s[SIZE];
#endif
	word	XYin[2 * SIZE], Zin[SIZE], Xout[SIZE], Yout[SIZE], Zout[SIZE];
	word*	recvX, recvY;
	uint8_t ad[FIELD_TYPE_NB + FIELD_LENGTH_NB + AEGIS_IV_NB + FIELD_TARGET_NB + FIELD_SEQNB_NB + FIELD_KEP2_SIGN_OF];
	uint8_t expectedMAC[AEGIS_MAC_NB];
	uint8_t	signedMessage[4 * SIZE * sizeof(word)];
	word	signResult;

	/* Scalar multiplication */
	recvX = session->kep.receivedPointXY;
	recvY = session->kep.receivedPointXY + SIZE;
	wordArrayToByteArray(recvX, session->receivedMessage.data + FIELD_KEP2_BGX_OF, SIZE * sizeof(word));
	wordArrayToByteArray(recvY, session->receivedMessage.data + FIELD_KEP2_BGY_OF, SIZE * sizeof(word));
	ECDHPointMultiply(session->kep.scalar, recvX, recvY, XYin, XYin + SIZE);

	/* Compute session key */
#if (ENDIAN_CONVERT)
	wordArrayToByteArray(shaBuff, XYin, 2 * SIZE * sizeof(word));
	sha3_HashBuffer(256, SHA3_FLAGS_NONE, shaBuff, 2 * SIZE * sizeof(word), session->sessionKey, AEGIS_KEY_NB);
#else
	sha3_HashBuffer(256, SHA3_FLAGS_NONE, XYin, 2 * SIZE * sizeof(word), session->sessionKey, AEGIS_KEY_NB);
#endif

	/* Decrypt and verify MAC + create AEGIS ctx for first time */
	init_AEGIS_ctx_IV(&session->aegisCtx, session->sessionKey, session->receivedMessage.IV);
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
	memcpy(signedMessage, session->receivedMessage.data + FIELD_KEP2_BGX_OF, 2 * SIZE * sizeof(word));
	wordArrayToByteArray(signedMessage + 2 * SIZE, session->kep.generatedPointXY, 2 * SIZE * sizeof(word));
#if (ENDIAN_CONVERT)
	wordArrayToByteArray(r, session->receivedMessage.data + FIELD_KEP2_SIGN_OF, 2 * SIZE * sizeof(word));
	wordArrayToByteArray(s, session->receivedMessage.data + FIELD_KEP2_SIGN_OF + 2 * SIZE * sizeof(word), 2 * SIZE * sizeof(word));
	signResult = ecdsaCheck(signedMessage, 4 * SIZE * sizeof(word), pkxDrone, pkyDrone, r, s);
#else
	signResult = ecdsaCheck(signedMessage, 4 * SIZE * sizeof(word), pkxDrone, pkyDrone,
		session->receivedMessage.data, session->receivedMessage.data + 2 * SIZE * sizeof(word));
#endif

	/* Manage administration */
	if (signResult) {
		addOneSeqNb(&session->sequenceNb);
		session->kep.cachedMessageValid = 0;
		session->kep.numTransmissions = 0;
		session->kep.timeOfTransmission = 0;
	}

	return !signResult;
}

word KEP3_compute_handlerBaseStation(struct SessionInfo* session) {
	word	messageToSign[4 * SIZE * sizeof(word)];

	memcpy(messageToSign, session->kep.generatedPointXY, 2 * SIZE * sizeof(word));
	memcpy(messageToSign + 2 * SIZE, session->kep.receivedPointXY, 2 * SIZE * sizeof(word));
	ecdsaSign(messageToSign, 4 * SIZE * sizeof(word), privBS, session->kep.signature, session->kep.signature + SIZE);

	return 0;
}

word KEP3_send_handlerBaseStation(struct SessionInfo* session) {
#if (ENDIAN_CONVERT)
	signed_word i, j;
	uint8_t		lengthArr[4];
#endif
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

#if (ENDIAN_CONVERT)
		wordArrayToByteArray(lengthArr, &length, 4);
		index = encodeMessage(session->kep.cachedMessage, TYPE_KEP3_SEND, lengthArr, session->targetID, session->sequenceNb, IV);
#else
		index = encodeMessage(session->kep.cachedMessage, TYPE_KEP3_SEND, &length, session->targetID, session->sequenceNb, IV);
#endif
		/* Put data in */
		wordArrayToByteArray(session->kep.cachedMessage + index, session->kep.signature, 2 * SIZE * sizeof(word));

		/* Encrypt and MAC */
		setIV(&session->aegisCtx, IV);
		aegisEncryptMessage(&session->aegisCtx, session->kep.cachedMessage, index, FIELD_KEP3_SIGN_NB);

		session->kep.cachedMessageValid = 1;
	}

	/* Send message */
	while (transmit(&session->IO, session->kep.cachedMessage, KEP3_MESSAGE_BYTES, 1) == -1);

	/* Manage administration */
	session->kep.numTransmissions++;
	session->kep.timeOfTransmission = clock();
	return 0;
}

word KEP3_wait_handlerBaseStation(struct SessionInfo* session) {
	double_word currentTime;
	double_word elapsedTime;

	currentTime = (double_word)clock();
	elapsedTime = ((float_word)currentTime - session->kep.timeOfTransmission) / CLOCKS_PER_SEC;
	if (elapsedTime > KEP_RETRANSMISSION_TIMEOUT)
		return -1;

	return session->receivedMessage.type == TYPE_KEP3_SEND;
}

word KEP5_verify_handlerBaseStation(struct SessionInfo* session) {

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

	default:
		return KEP_idle;
	}
}

/* cache messages */