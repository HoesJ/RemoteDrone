#include "./../../include/sm/session_init_sm.h"

void init_COMM_ctx(struct MESS_ctx* ctx) {
	ctx->sendType = TYPE_COMM_SEND;
	ctx->ackType = TYPE_COMM_ACK;
	ctx->nackType = TYPE_COMM_NACK;

	ctx->nackLength = SESSION_NACK_MESSAGE_BYTES;
	ctx->ackLength = SESSION_ACK_MESSAGE_BYTES;

	ctx->needsAcknowledge = 1;
	ctx->numTransmissions = 0;
	ctx->timeOfTransmission = 0;

	ctx->checkInputFunction = &checkCommInput;
	ctx->writeOutputFunction = &writeCommOutput;
}

void init_STAT_ctx(struct MESS_ctx* ctx) {
	ctx->sendType = TYPE_STAT_SEND;
	ctx->ackType = TYPE_STAT_ACK;
	ctx->nackType = TYPE_STAT_NACK;

	ctx->nackLength = SESSION_NACK_MESSAGE_BYTES;
	ctx->ackLength = SESSION_ACK_MESSAGE_BYTES;

	ctx->needsAcknowledge = 1;
	ctx->numTransmissions = 0;
	ctx->timeOfTransmission = 0;

	ctx->checkInputFunction = &checkStatInput;
	ctx->writeOutputFunction = &writeStatOutput;
}

void init_FEED_ctx(struct MESS_ctx* ctx) {
	ctx->sendType = TYPE_FEED_SEND;
	ctx->ackType = 0;
	ctx->nackType = 0;

	ctx->nackLength = 0;
	ctx->ackLength = 0;

	ctx->needsAcknowledge = 0;
	ctx->numTransmissions = 0;
	ctx->timeOfTransmission = 0;

	ctx->checkInputFunction = &checkFeedInput;
	ctx->writeOutputFunction = &writeFeedOutput;
}