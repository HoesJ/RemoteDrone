#include "./../include/session_req_sm.h"

signed_word MESS_idle_handlerReq(struct SessionInfo* session, struct MESS_ctx* ctx) {
	size_t inputLength;

	inputLength = ctx->checkInputFunction(ctx->cachedMessage + FIELD_HEADER_NB, DECODER_BUFFER_SIZE);
	if (inputLength > 0)
		ctx->inputDataValid = 1;
	return 1;
}

signed_word MESS_encrypt_handlerReq(struct SessionInfo* session, struct MESS_ctx* ctx) {
	uint8_t IV[AEGIS_IV_NB];

	if (!ctx->inputDataValid)
		return 0;

	/* Encode and assume input data sits at correct position */
	getRandomBytes(AEGIS_IV_NB, IV);
	encodeMessage(ctx->cachedMessage, ctx->sendType, ctx->sendLength, session->targetID, session->sequenceNb, IV);

	/* Encrypt */
	session->aegisCtx.iv = IV;
	aegisEncryptMessage(&session->aegisCtx, ctx->cachedMessage, FIELD_HEADER_NB, ctx->sendLength - FIELD_HEADER_NB - AEGIS_MAC_NB);

	/* Set valid */
	ctx->inputDataValid = 0;
	ctx->cachedMessageValid = 1;
	ctx->numTransmissions = 0;

	return 1;
}

signed_word MESS_send_handlerReq(struct SessionInfo* session, struct MESS_ctx* ctx) {
	uint32_t	length;

	if (ctx->numTransmissions >= SESSION_MAX_RETRANSMISSIONS) {
		printf("Max retransmissions reached on %x\n", ctx->sendType);
		return 0;
	}

	if (!ctx->cachedMessageValid)
		return 0;

	/* Send message */
	while (transmit(&session->IO, ctx->sendLength, length, 1) == -1);

	/* Manage administration */
	ctx->numTransmissions++;
	ctx->timeOfTransmission = clock();
	addOneSeqNb(&session->sequenceNb);

	return 1;
}

signed_word MESS_wait_handlerReq(struct SessionInfo* session, struct MESS_ctx* ctx) {
	double_word  currentTime;
	double_word  elapsedTime;

	currentTime = (double_word)clock();
	elapsedTime = ((float_word)currentTime - ctx->timeOfTransmission) / CLOCKS_PER_SEC;
	if (elapsedTime > SESSION_RETRANSMISSION_TIMEOUT)
		return -1;

	if (session->receivedMessage.messageStatus == Message_valid && *session->receivedMessage.type == ctx->ackType)
		return 1;
	else if (session->receivedMessage.messageStatus == Message_valid && *session->receivedMessage.type == ctx->nackType) {
		/* Verify correctness of NACK - content */
		addOneSeqNb(&session->receivedMessage.ackSeqNbNum);
		if (session->sequenceNb != session->receivedMessage.ackSeqNbNum)
			return 0;

		/* Verify correctness of NACK - MAC */
		session->aegisCtx.iv = session->receivedMessage.IV;
		return - aegisDecryptMessage(&session->aegisCtx, session->receivedMessage.message, ctx->nackLength - AEGIS_MAC_NB, 0);
	}
	else
		return 0;
}

signed_word MESS_verify_handlerReq(struct SessionInfo* session, struct MESS_ctx* ctx) {
	/* Verify ACked seqNB */
	addOneSeqNb(&session->receivedMessage.ackSeqNbNum);
	if (session->sequenceNb != session->receivedMessage.ackSeqNbNum)
		return 0;

	/* verify MAC */
	session->aegisCtx.iv = session->receivedMessage.IV;
	return aegisDecryptMessage(&session->aegisCtx, session->receivedMessage.message, ctx->ackLength - AEGIS_MAC_NB, 0);
}


messState messReqContinue(struct SessionInfo* session, struct MESS_ctx* ctx, messState currentState) {

	switch (currentState)
	{
	case MESS_idle:
		return MESS_idle_handlerReq(session, ctx) ? MESS_encrypt : MESS_idle;

	case MESS_encrypt:
		return MESS_encrypt_handlerReq(session, ctx) ? MESS_send : MESS_idle;

	case MESS_send:
		return MESS_send_handlerReq(session, ctx) ? (ctx->needsAcknowledge ? MESS_wait : MESS_idle) : MESS_idle;

	case MESS_wait:
		switch (MESS_wait_handlerReq(session, ctx)) {
		case -1: return MESS_send;
		case 0:	 return MESS_wait;
		case 1:  return MESS_verify;
		default: return MESS_idle;
		}

	case MESS_verify:
		return MESS_verify_handlerReq(session, ctx) ? MESS_idle : MESS_send;

	default:
		return MESS_idle;
		break;
	}
}

