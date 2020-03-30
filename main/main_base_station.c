#include "./../include/main_base_station.h"

void initializeBaseSession(struct SessionInfo* session, int txPipe, int rxPipe) {
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
	session->targetID[0] = 1;

	/* Initialize own target ID */
	memset(session->ownID, 0, FIELD_TARGET_NB);
}

void clearSession(struct SessionInfo* session) {
	/* Re-Initialize state */
	session->state.systemState = Idle;
	session->state.kepState = KEP_idle;
	session->state.commState = COMM_idle;
	session->state.statState = STAT_idle;
	session->state.feedState = FEED_idle;

	/* Re-Initialize KEP ctx */
	init_KEP_ctx(&session->kep);

	/* Re-Initialize sequence NB */
	getRandomBytes(sizeof(word), &session->sequenceNb);
}

void stateMachine(struct SessionInfo* session, uint8_t receivedMessage, struct externalBaseStationCommands* external) {
	switch (session->state.systemState) {
	case Idle:
		if (external->start)
			session->state.systemState = KEP;
		else
			session->state.systemState = Idle;
		break;

	case KEP:
		if (!external->quit) {
			/* Sets ClearSession if something goes wrong */
			session->state.kepState = kepContinue(session, session->state.kepState);

			/* If KEP is done, go to next state */
			if (session->state.kepState == Done)
				session->state.systemState = SessionReady;
		}
		else
			session->state.systemState = ClearSession;
		break;

	case SessionReady:
		/* Complex logic on which state machine to give control */
		/* If external say a command needs to be send -> give to COMM */
		/* If external says to quit, go to clear session */
		/* Poll the receiver buffer, give control to whatever has received stuff */
		/* Extra complexity, need to check retransmission timer so need to give control to waiting states as well */
		break;

	case ClearSession:
		/* Clear session and go to idle state */
		clearSession(session);
		break;

	default:
		session->state.systemState = ClearSession;
		break;
	}
}

void setExternalBSCommands(struct externalBaseStationCommands* external, uint8_t key) {
	switch (key) {
	case 's':
		external->start = 1;
		break;
	case 'q':
		external->quit = 1;
		break;
	case 'c':
		external->sendCommand = 1;
		break;
	default:
		external->start = 0;
		external->quit = 0;
		external->sendCommand = 0;
		break;
	}
}

void loopBS(struct SessionInfo* session, struct externalBaseStationCommands* external) {
	uint8_t key, receivedType;
	uint8_t command[256];
	word	externalOn;

	externalOn = 0;
	while (1) {
		/* Deal with external commands */
		if (kbhit()) {
			key = getch();
			setExternalBSCommands(external, key);
			if (external->sendCommand) {
				printf("Please enter the command:\n");
				scanf("%s", external->command);
			}
			externalOn = 1;
		}
		else if (externalOn) {
			/* Clear external signals */
			setExternalBSCommands(external, '\0');
			externalOn = 0;
		}

		/* Poll receiver */
		receivedType = pollAndDecode(session);

		/* Hand control to state machine */
		stateMachine(session, receivedType, external);
	}
}

int main_base_station(int txPipe, int rxPipe) {
	struct SessionInfo session;
	struct externalBaseStationCommands external;

	initializeBaseSession(&session, txPipe, rxPipe);
	setExternalBSCommands(&external, '\0');

	loopBS(&session, &external);

	return 0;
}

#if WINDOWS
int main_base_station_win(struct threadParam *params) {
	struct SessionInfo session;
	struct externalBaseStationCommands external;

	initializeBaseSession(&session, (int)params->txPipe, (int)params->rxPipe);
	setExternalBSCommands(&external, '\0');

	loopBS(&session, &external);
}
#endif