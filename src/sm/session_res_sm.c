#include "./../../include/sm/session_res_sm.h"

int8_t MESS_idle_handlerRes(struct SessionInfo* session, struct MESS_ctx* ctx) {
	/* Check for incomming request */
	ctx->reactedToSeqNbReq = 0;
	return session->receivedMessage.messageStatus == Message_valid && *session->receivedMessage.type == ctx->sendType;
}

int8_t MESS_verify_handlerRes(struct SessionInfo* session, struct MESS_ctx* ctx) {
	/* Verify MAC */
	session->aegisCtx.iv = session->receivedMessage.IV;
	return aegisDecryptMessage(&session->aegisCtx, session->receivedMessage.message, FIELD_HEADER_NB,
							   session->receivedMessage.lengthNum - FIELD_HEADER_NB - AEGIS_MAC_NB);
}

int8_t MESS_react_handlerRes(struct SessionInfo* session, struct MESS_ctx* ctx) {
	/* Check if you already have reacted */
	if (ctx->reactedToSeqNbReq == session->receivedMessage.seqNbNum)
		return 1;

	ctx->writeOutputFunction(session->receivedMessage.data, session->receivedMessage.lengthNum - MIN_MESSAGE_BYTES);

	ctx->reactedToSeqNbReq = session->receivedMessage.seqNbNum;
	return 1;
}

int8_t MESS_ack_handlerRes(struct SessionInfo* session, struct MESS_ctx* ctx) {
	word index;
	uint8_t IV[AEGIS_IV_NB];

	/* Encode NACK on received message */
	getRandomBytes(AEGIS_IV_NB, IV);
	addOneSeqNb(&ctx->sequenceNb);
	index = encodeMessage(ctx->cachedMessage, ctx->ackType, ctx->ackLength, session->targetID, ctx->sequenceNb, IV);

	/* Put data in */
	memcpy(ctx->cachedMessage + index, session->receivedMessage.seqNb, FIELD_SEQNB_NB);

	/* Encrypt */
	session->aegisCtx.iv = IV;
	aegisEncryptMessage(&session->aegisCtx, ctx->cachedMessage, FIELD_HEADER_NB + FIELD_SEQNB_NB, 0);

	/* Set valid */
	ctx->inputDataValid = 0;
	ctx->cachedMessageValid = 1;
	ctx->numTransmissions = 0;

	return 1;
}

int8_t MESS_send_handlerRes(struct SessionInfo* session, struct MESS_ctx* ctx) {
	uint32_t	length;

	if (ctx->numTransmissions >= SESSION_MAX_RETRANSMISSIONS) {
		printf("Max retransmissions reached on %x\n", ctx->sendType);
		return 0;
	}

	if (!ctx->cachedMessageValid)
		return 0;

	/* Send message */
	length = (ctx->cachedMessage[0] == ctx->ackType) ? ctx->ackLength : ctx->nackLength;
	while (transmit(&session->IO, ctx->cachedMessage, length, 1) == -1);

	/* Manage administration */
	ctx->numTransmissions++;
	ctx->timeOfTransmission = clock();

	return 1;
}

int8_t MESS_timewait_handlerRes(struct SessionInfo* session, struct MESS_ctx* ctx) {
	double_word  currentTime;
	double_word  elapsedTime;

	currentTime = (double_word)clock();
	elapsedTime = ((float_word)currentTime - ctx->timeOfTransmission ) / CLOCKS_PER_SEC;
	if (elapsedTime > 2 * SESSION_RETRANSMISSION_TIMEOUT)
		return 1;

	if (session->receivedMessage.messageStatus == Message_valid && *session->receivedMessage.type == ctx->sendType)
		return -1;
	else
		return 0;
}

int8_t MESS_nack_handler(struct SessionInfo* session, struct MESS_ctx* ctx) {
	word index;
	uint8_t IV[AEGIS_IV_NB];

	/* Encode NACK on received message */
	getRandomBytes(AEGIS_IV_NB, IV);
	index = encodeMessage(ctx->cachedMessage, ctx->nackType, ctx->nackLength, session->targetID, ctx->sequenceNb, IV);

	/* Put data in */
	memcpy(ctx->cachedMessage + index, session->receivedMessage.seqNb, FIELD_SEQNB_NB);

	/* Encrypt */
	session->aegisCtx.iv = IV;
	aegisEncryptMessage(&session->aegisCtx, ctx->cachedMessage, FIELD_HEADER_NB + FIELD_SEQNB_NB, 0);

	/* Set valid */
	ctx->inputDataValid = 0;
	ctx->cachedMessageValid = 1;
	ctx->numTransmissions = 0;

	return 1;
}

/* Public functions */

messState messResContinue(struct SessionInfo* session, struct MESS_ctx* ctx, messState currentState) {
	switch (currentState) {
	case MESS_idle:
		return MESS_idle_handlerRes(session, ctx) ? MESS_verify : MESS_idle;

	case MESS_verify:
		return MESS_verify_handlerRes(session, ctx) ? MESS_react : (ctx->needsAcknowledge ? MESS_nack : MESS_idle);

	case MESS_react:
		return MESS_react_handlerRes(session, ctx) ? (ctx->needsAcknowledge ? MESS_ack : MESS_idle) : MESS_react;

	case MESS_ack:
		return MESS_ack_handlerRes(session, ctx) ? MESS_send : MESS_ack;

	case MESS_send:
		return MESS_send_handlerRes(session, ctx) ? MESS_timewait : MESS_idle;

	case MESS_timewait:
		switch (MESS_timewait_handlerRes(session, ctx)) {
		case -1: return MESS_verify;
		case 0:  return MESS_timewait;
		case 1:  return MESS_idle;
		default: return MESS_idle;
		}

	case MESS_nack:
		return MESS_nack_handler(session, ctx) ? MESS_send : MESS_nack;

	default:
		return MESS_idle;
	}
}