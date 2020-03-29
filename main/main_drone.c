#include "./../include/main_drone.h"

void initializeDroneSession(struct SessionInfo* session, int txPipe, int rxPipe) {
	/* Initialize state */
	session->state.systemState = Idle;
	session->state.kepState = KEP_idle;
	session->state.commState = COMM_idle;
	session->state.statState = STAT_idle;
	session->state.feedState = FEED_idle;

	/* Initialize IO ctx */
	init_IO_ctx(&session->IO, txPipe, rxPipe);

	/* Initialize KEP ctx */
	init_KEP_ctx(&session->kep);

	/* Initialize session key */

	/* Initialize sequence NB */
	getRandomBytes(sizeof(word), &session->sequenceNb);

	/* Initialize expected sequence NB */
	memset(session->expectedSequenceNb, 0, FIELD_SEQNB_NB);

	/* Initialize target ID */
	memset(session->targetID, 0, FIELD_TARGET_NB);

	/* Initialize own target ID */
	memset(session->ownID, 0, FIELD_TARGET_NB);
	session->ownID[0] = 1;
}

int main_drone(int txPipe, int rxPipe) {
	return 0;
}

#if WINDOWS
#include "./../include/enc_dec.h"
#include "./../include/print_number.h"
int main_drone_win(struct threadParam* params) {
	struct SessionInfo session;
	uint8_t type = 0;

	initializeDroneSession(&session, (int)params->txPipe, (int)params->rxPipe);

	printf("Drone -\twaiting for incomming stuff\n");
	while (type == 0) {
		type = pollAndDecode(&session);
	}

	printf("Drone -\tReceived");
	printf("\n\tType:\t%d", session.receivedMessage.type);
	printf("\n\tLength:\t%d", session.receivedMessage.length);
	printf("\n\tIV:\t");
	printNumber(session.receivedMessage.IV, 16 / 4);
	printf("\n\tTarget:\t%d", session.receivedMessage.targetID);
	printf("\n\tSeq:\t%d", session.receivedMessage.seqNb);
	printf("\n\tdata:\t");
	printNumber(session.receivedMessage.data, 64 / 4);
	printf("\n\tMAC:\t");
	printNumber(session.receivedMessage.MAC, 16 / 4);
}
#endif