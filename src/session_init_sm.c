#include "./../include/session_init_sm.h"

void init_COMM_ctx(struct MESS_ctx* ctx) {
	ctx->sendType = TYPE_COMM_SEND;
	ctx->ackType = TYPE_COMM_ACK;
	ctx->nackType = TYPE_COMM_NACK;
	ctx->anackType = TYPE_COMM_ANACK;

	ctx->sendLength = COMM_MESSAGE_BYTES;
	ctx->nackLength = SESSION_NACK_MESSAGE_BYTES;
	ctx->ackLength = SESSION_ACK_MESSAGE_BYTES;

	ctx->needsAcknowledge = 1;
	ctx->numTransmissions = 0;
	ctx->timeOfTransmission = 0;

	ctx->inputDataValid = 0;
	ctx->cachedMessageValid = 0;
}

void init_STAT_ctx(struct MESS_ctx* ctx) {
	ctx->sendType = TYPE_STAT_SEND;
	ctx->ackType = TYPE_STAT_ACK;
	ctx->nackType = TYPE_STAT_NACK;
	ctx->anackType = TYPE_STAT_ANACK;

	ctx->sendLength = STAT_MESSAGE_BYTES;
	ctx->nackLength = SESSION_NACK_MESSAGE_BYTES;
	ctx->ackLength = SESSION_ACK_MESSAGE_BYTES;

	ctx->needsAcknowledge = 1;
	ctx->numTransmissions = 0;
	ctx->timeOfTransmission = 0;

	ctx->inputDataValid = 0;
	ctx->cachedMessageValid = 0;
}

void init_FEED_ctx(struct MESS_ctx* ctx) {
	ctx->sendType = TYPE_FEED_SEND;
	ctx->ackType = NULL;
	ctx->nackType = NULL;
	ctx->anackType = NULL;

	ctx->sendLength = FEED_MESSAGE_BYTES;
	ctx->nackLength = NULL;
	ctx->ackLength = NULL;

	ctx->needsAcknowledge = 0;
	ctx->numTransmissions = 0;
	ctx->timeOfTransmission = 0;

	ctx->inputDataValid = 0;
	ctx->cachedMessageValid = 0;
}