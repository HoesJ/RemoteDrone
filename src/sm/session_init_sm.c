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
#if RELIABLE_FEED
	ctx->ackType = TYPE_FEED_ACK;
	ctx->nackType = TYPE_FEED_NACK;

	ctx->nackLength = SESSION_NACK_MESSAGE_BYTES;
	ctx->ackLength = SESSION_ACK_MESSAGE_BYTES;

	ctx->needsAcknowledge = 1;
#else
	ctx->ackType = 0;
	ctx->nackType = 0;

	ctx->nackLength = 0;
	ctx->ackLength = 0;

	ctx->needsAcknowledge = 0;
#endif
	ctx->numTransmissions = 0;
	ctx->timeOfTransmission = 0;

	ctx->checkInputFunction = &checkFeedInput;
	ctx->writeOutputFunction = &writeFeedOutput;
}

void init_ALIVE_ctx(struct MESS_ctx* ctx) {
	ctx->sendType = TYPE_ALIVE_SEND;
	ctx->ackType = TYPE_ALIVE_ACK;
	ctx->nackType = TYPE_ALIVE_NACK;

	ctx->nackLength = SESSION_NACK_MESSAGE_BYTES;
	ctx->ackLength = SESSION_ACK_MESSAGE_BYTES;

	ctx->needsAcknowledge = 1;
	ctx->numTransmissions = 0;
	ctx->timeOfTransmission = 0;

	ctx->checkInputFunction = &checkAliveInput;
	ctx->writeOutputFunction = &writeAliveOutput;
}