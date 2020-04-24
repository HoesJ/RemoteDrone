#include "./../include/main/main_base_station.h"

void initializeBaseSession(struct SessionInfo* session, int txPipe, int rxPipe) {
	/* Initialize state */
	session->state.systemState = Idle;
	session->state.kepState = KEP_idle;
	session->state.commState = MESS_idle;
	session->state.statState = MESS_idle;
	session->state.feedState = MESS_idle;

	/* Initialize IO ctx */
	init_IO_ctx(&session->IO, txPipe, rxPipe);

	/* Initialize KEP ctx */
	init_KEP_ctxBaseStation(&session->kep);

	/* Initialize COMM ctx */
	init_COMM_ctx(&session->comm);

	/* Initialize STAT ctx */
	init_STAT_ctx(&session->stat);

	/* Initialize FEED ctx */
	init_FEED_ctx(&session->feed);

	/* Initialize target ID */
	memset(session->targetID, 0, FIELD_TARGET_NB);
	session->targetID[0] = 1;

	/* Initialize own target ID */
	memset(session->ownID, 0, FIELD_TARGET_NB);

	/* Initialize message status. */
	session->receivedMessage.messageStatus = Channel_empty;

	/* Make pipe non-blocking. */
#if UNIX
	fcntl(rxPipe, F_SETFL, O_NONBLOCK);
#endif
}

void initializeSessionSequenceNbsBasestation(struct SessionInfo *session) {
	session->comm.sequenceNb = session->kep.sequenceNb;
	session->comm.expectedSequenceNb = session->kep.expectedSequenceNb;

	session->stat.sequenceNb = session->kep.sequenceNb;
	session->stat.expectedSequenceNb = session->kep.expectedSequenceNb;

	session->feed.sequenceNb = session->kep.sequenceNb;
	session->feed.expectedSequenceNb = session->kep.expectedSequenceNb;
}

void clearSessionBasestation(struct SessionInfo* session) {
	/* Re-Initialize state */
	session->state.systemState = Idle;
	session->state.kepState = KEP_idle;
	session->state.commState = MESS_idle;
	session->state.statState = MESS_idle;
	session->state.feedState = MESS_idle;

	/* Re-Initialize KEP ctx */
	init_KEP_ctxBaseStation(&session->kep);

	/* Re-Initialize COMM ctx */
	init_COMM_ctx(&session->comm);

	/* Re-Initialize STAT ctx */
	init_STAT_ctx(&session->stat);

	/* Re-Initialize FEED ctx */
	init_FEED_ctx(&session->feed);
}

void stateMachineBaseStation(struct SessionInfo* session, struct externalCommands* external) {
	switch (session->state.systemState) {
	case Idle:
		if (external->start)
			session->state.systemState = KEP;
		else
			session->state.systemState = Idle;
		break;

	case KEP:
		if (!external->quit) {

			/* Look at the receiver pipe */
			if (session->receivedMessage.messageStatus != Message_valid)
				pollAndDecode(session);
			else
				printf("BS\t- current KEP state: %d\n", session->state.kepState);

			/* Sets ClearSession if something goes wrong */
			session->state.kepState = kepContinueBaseStation(session, session->state.kepState);

			/* If KEP is done, go to next state */
			if (session->state.kepState == Done) {
				session->state.systemState = SessionReady;
				initializeSessionSequenceNbsBasestation(session);
				printf("BS\t- session ready\n");
			}
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
		if (!external->quit) {
			if (session->receivedMessage.messageStatus != Message_valid)
				pollAndDecode(session);

			if (session->state.commState != MESS_idle && session->state.commState != MESS_wait)
				printf("BS\t- current COMM state: %d\n", session->state.commState);
			if (session->state.statState != MESS_idle && session->state.statState != MESS_timewait)
				printf("BS\t- current STAT state: %d\n", session->state.statState);
			if (session->state.feedState != MESS_idle)
				printf("BS\t- current FEED state: %d\n", session->state.feedState);
			

			if (session->receivedMessage.messageStatus == Message_valid) {
				if ((*session->receivedMessage.type & 0xc0) == (TYPE_COMM_SEND & 0xc0)) {
					session->state.commState = messReqContinue(session, &session->comm, session->state.commState);
					session->state.statState = messResContinue(session, &session->stat, session->state.statState);
					session->state.feedState = messResContinue(session, &session->feed, session->state.feedState);
				}
				else if ((*session->receivedMessage.type & 0xc0) == (TYPE_STAT_SEND & 0xc0)) {
					session->state.statState = messResContinue(session, &session->stat, session->state.statState);
					session->state.commState = messReqContinue(session, &session->comm, session->state.commState);
					session->state.feedState = messResContinue(session, &session->feed, session->state.feedState);
				}
				else if ((*session->receivedMessage.type & 0xc0) == (TYPE_FEED_SEND & 0xc0)) {
					session->state.feedState = messResContinue(session, &session->feed, session->state.feedState);
					session->state.commState = messReqContinue(session, &session->comm, session->state.commState);
					session->state.statState = messResContinue(session, &session->stat, session->state.statState);
				}
			}
			else {
				session->state.commState = messReqContinue(session, &session->comm, session->state.commState);
				session->state.statState = messResContinue(session, &session->stat, session->state.statState);
				session->state.feedState = messResContinue(session, &session->feed, session->state.feedState);
			}
		}
		else
			session->state.systemState = ClearSession;
		break;

	case ClearSession:
		/* Clear session and go to idle state */
		clearSessionBasestation(session);
		break;

	default:
		session->state.systemState = ClearSession;
		break;
	}
}

void setExternalBaseStationCommands(struct externalCommands* external, uint8_t key) {
	switch (key) {
	case 's':
		external->start = 1;
		external->quit = 0;
		break;
	case 'q':
		external->start = 0;
		external->quit = 1;
		break;
	default:
		external->start = 0;
		external->quit = 0;
		break;
	}
}

void loopBaseStation(struct SessionInfo* session, struct externalCommands* external) {
	uint8_t key;

	while (1) {
		/* Deal with external commands */
		if (kbhit()) {
			key = readChar();
			setExternalBaseStationCommands(external, key);
		}
		else {
			setExternalBaseStationCommands(external, '\0');
		}

		/* Hand control to state machine */
		stateMachineBaseStation(session, external);
	}
}

int main_base_station(int txPipe, int rxPipe) {
	struct SessionInfo session;
	struct externalCommands external;

	initializeBaseSession(&session, txPipe, rxPipe);
	setExternalBaseStationCommands(&external, '\0');

	loopBaseStation(&session, &external);

	return 0;
}

#if WINDOWS
int main_base_station_win(struct threadParam *params) {
	struct SessionInfo session;
	struct externalCommands external;

	initializeBaseSession(&session, (int)params->txPipe, (int)params->rxPipe);
	setExternalBaseStationCommands(&external, '\0');

	loopBaseStation(&session, &external);
}
#endif