#include "./../../include/sm/session_req_sm.h"

/**
 * 0: idle
 * 1: MESS_encrypt
 */
int8_t MESS_idle_handlerReq(struct SessionInfo* session, struct MESS_ctx* ctx) {
	size_t inputLength;

	inputLength = ctx->checkInputFunction(ctx->cachedMessage + FIELD_HEADER_NB, DECODER_BUFFER_SIZE);
	if (inputLength > 0) {
		ctx->sendLength = FIELD_HEADER_NB + inputLength + AEGIS_MAC_NB;
		return 1;
	}
	else
		return 0;
}

/**
 * 0: MESS_idle
 * 1: MESS_send
 */
int8_t MESS_encrypt_handlerReq(struct SessionInfo* session, struct MESS_ctx* ctx) {
	uint8_t IV[AEGIS_IV_NB];

	/* Encode and assume input data sits at correct position */
	getRandomBytes(AEGIS_IV_NB, IV);
	addOneSeqNb(&ctx->sequenceNb);
	encodeMessage(ctx->cachedMessage, ctx->sendType, ctx->sendLength, session->targetID, ctx->sequenceNb, IV);

	/* Encrypt */
	session->aegisCtx.iv = IV;
	aegisEncryptMessage(&session->aegisCtx, ctx->cachedMessage, FIELD_HEADER_NB, ctx->sendLength - FIELD_HEADER_NB - AEGIS_MAC_NB);

	/* Set valid */
	ctx->numTransmissions = 0;

	return 1;
}

/**
 * 0: MESS_idle
 * 1: MESS_wait
 */
int8_t MESS_send_handlerReq(struct SessionInfo* session, struct MESS_ctx* ctx) {
	if (ctx->numTransmissions >= SESSION_MAX_RETRANSMISSIONS) {
		printf("Max retransmissions reached on %x\n", ctx->sendType);
		session->state.systemState = ClearSession;
		return 0;
	}

	/* Send message */
	transmit(&session->IO, ctx->cachedMessage, ctx->sendLength, 1);

	/* Manage administration */
	ctx->numTransmissions++;
	ctx->timeOfTransmission = clock();

	return ctx->needsAcknowledge ? 1 : 0;
}

/**
 * -1: MESS_send
 *  0: MESS_wait
 *  1: MESS_verify
 */
int8_t MESS_wait_handlerReq(struct SessionInfo* session, struct MESS_ctx* ctx) {
	double_word currentTime;
	double_word elapsedTime;

	currentTime = (double_word)clock();
	elapsedTime = ((float_word)currentTime - ctx->timeOfTransmission) / CLOCKS_PER_SEC;
	if (elapsedTime > SESSION_RETRANSMISSION_TIMEOUT)
		return -1;

	if (session->receivedMessage.messageStatus == Message_valid && (*session->receivedMessage.type == ctx->ackType))
		return 1;
	else if (session->receivedMessage.messageStatus == Message_valid && (*session->receivedMessage.type == ctx->nackType)) {
		/* This function will use the received message. */
		session->receivedMessage.messageStatus = Message_used;
		
		/* Verify correctness of NACK - content */
		if (ctx->sequenceNb != session->receivedMessage.ackSeqNbNum)
			return 0;

		/* Verify correctness of NACK - MAC */
		session->aegisCtx.iv = session->receivedMessage.IV;
		return -aegisDecryptMessage(&session->aegisCtx, session->receivedMessage.message,
									session->receivedMessage.lengthNum - AEGIS_MAC_NB, 0);
	} else if ((session->receivedMessage.messageStatus == Message_valid || session->receivedMessage.messageStatus == Message_repeated) &&
			   ((*session->receivedMessage.type & 0xc0) == (ctx->sendType & 0xc0))) {
		/* This function will use the received message. */
		session->receivedMessage.messageStatus = Message_used;
	}

	return 0;
}

/**
 * 0: MESS_wait
 * 1: MESS_idle
 */
int8_t MESS_verify_handlerReq(struct SessionInfo* session, struct MESS_ctx* ctx) {
	/* This function will use the received message. */
	session->receivedMessage.messageStatus = Message_used;

	/* Verify ACked seqNB */
	if (ctx->sequenceNb != session->receivedMessage.ackSeqNbNum)
		return 0;

	/* verify MAC */
	session->aegisCtx.iv = session->receivedMessage.IV;
	if (aegisDecryptMessage(&session->aegisCtx, session->receivedMessage.message,
							session->receivedMessage.lengthNum - AEGIS_MAC_NB, 0)) {
		return 1;
	} else
		return 0;
}

/* Public functions */

messState messReqContinue(struct SessionInfo* session, struct MESS_ctx* ctx, messState currentState) {
	switch (currentState) {
	case MESS_idle:
		return MESS_idle_handlerReq(session, ctx) ? MESS_encrypt : MESS_idle;

	case MESS_encrypt:
		return MESS_encrypt_handlerReq(session, ctx) ? MESS_send : MESS_idle;

	case MESS_send:
		return MESS_send_handlerReq(session, ctx) ? MESS_wait : MESS_idle;

	case MESS_wait:
		switch (MESS_wait_handlerReq(session, ctx)) {
		case -1: return MESS_send;
		case 0:	 return MESS_wait;
		case 1:  return MESS_verify;
		default: return MESS_idle;
		}

	case MESS_verify:
		return MESS_verify_handlerReq(session, ctx) ? MESS_idle : MESS_wait;

	default:
		return MESS_idle;
	}
}