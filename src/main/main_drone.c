#include "./../../include/main/main_drone.h"

void initializeDroneSession(struct SessionInfo* session, int txPipe, int rxPipe) {
	/* Initialize state */
	session->state.systemState = Idle;
	session->state.kepState = KEP_idle;
	session->state.commState = MESS_idle;
	session->state.statState = MESS_idle;
	session->state.feedState = MESS_idle;

	/* Initialize IO ctx */
	init_IO_ctx(&session->IO, txPipe, rxPipe);

	/* Initialize ctx for messages */
	init_KEP_ctxDrone(&session->kep);
	init_COMM_ctx(&session->comm);
	init_STAT_ctx(&session->stat);
	init_FEED_ctx(&session->feed);
	init_ALIVE_ctx(&session->aliveReq);
	init_ALIVE_ctx(&session->aliveRes);

	/* Initialize target ID */
	memset(session->targetID, 0, FIELD_TARGET_NB);

	/* Initialize own target ID */
	memset(session->ownID, 0, FIELD_TARGET_NB);
	session->ownID[0] = 1;

	/* Initialize message status. */
	session->receivedMessage.messageStatus = Channel_empty;

	/* Set MICRO_INTERVAL. */
	MICRO_INTERVAL = 5000000;

	/* Make pipe non-blocking. */
#if UNIX
#if !UDP
	fcntl(rxPipe, F_SETFL, O_NONBLOCK);
#endif

	/* Stop feed */
	FEED_ACTIVE = 0;
#endif
}

void initializeSessionSequenceNbsDrone(struct SessionInfo *session) {
	session->comm.sequenceNb = session->kep.sequenceNb;
	session->comm.expectedSequenceNb = session->kep.expectedSequenceNb;

	session->stat.sequenceNb = session->kep.sequenceNb;
	session->stat.expectedSequenceNb = session->kep.expectedSequenceNb;

	session->feed.sequenceNb = session->kep.sequenceNb;
	session->feed.expectedSequenceNb = session->kep.expectedSequenceNb;

	session->aliveReq.sequenceNb = session->kep.sequenceNb;
	session->aliveReq.expectedSequenceNb = session->kep.expectedSequenceNb;
	session->aliveRes.sequenceNb = session->kep.sequenceNb;
	session->aliveRes.expectedSequenceNb = session->kep.expectedSequenceNb;
}

void clearSessionDrone(struct SessionInfo* session) {
	/* Re-Initialize state */
	session->state.systemState = Idle;
	session->state.kepState = KEP_idle;
	session->state.commState = MESS_idle;
	session->state.statState = MESS_idle;
	session->state.feedState = MESS_idle;
	session->state.reqAliveState = MESS_idle;
	session->state.resAliveState = MESS_idle;

	/* Re-Initialize KEP ctx */
	init_KEP_ctxDrone(&session->kep);

	/* Re-Initialize COMM ctx */
	init_COMM_ctx(&session->comm);

	/* Re-Initialize STAT ctx */
	init_STAT_ctx(&session->stat);

	/* Re-Initialize FEED ctx */
	init_FEED_ctx(&session->feed);

	/* Re-Initialize ALIVE ctx */
	init_ALIVE_ctx(&session->aliveReq);

	/* Re-Initialize ALIVE ctx */
	init_ALIVE_ctx(&session->aliveRes);
}

void stateMachineDrone(struct SessionInfo* session, struct externalCommands* external) {
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
			if (session->receivedMessage.messageStatus != Message_valid && session->receivedMessage.messageStatus != Message_repeated)
				pollAndDecode(session);


			if (session->state.kepState != KEP_idle && session->state.kepState != KEP1_wait && session->state.kepState != KEP2_wait
				&& session->state.kepState != KEP3_wait && session->state.kepState != KEP2_wait_request)
				printf("Drone\t- Current KEP state: %d\n", session->state.kepState);

			/* Sets ClearSession if something goes wrong */
			session->state.kepState = kepContinueDrone(session, session->state.kepState);

			/* If KEP is done, go to next state */
			if (session->state.kepState == Done) {
				LAST_CHECK = getMicrotime();
				session->state.systemState = SessionReady;
				initializeSessionSequenceNbsDrone(session);
				printf("Drone\t- Session ready\n");
			}
		}
		else
			session->state.systemState = ClearSession;
		break;

	case SessionReady:
		if (!external->quit) {
			if (session->receivedMessage.messageStatus != Message_valid && session->receivedMessage.messageStatus != Message_repeated)
				pollAndDecode(session);
			
			if (session->state.commState != MESS_idle)
				printf("Drone\t- Current COMM state: %d\n", session->state.commState);
			if (session->state.statState != MESS_idle && session->state.statState != MESS_wait)
				printf("Drone\t- Current STAT state: %d\n", session->state.statState);
#if FEED_DEBUG
			if (session->state.feedState != MESS_idle && session->state.feedState != MESS_wait)
				printf("Drone\t- Current FEED state: %d\n", session->state.feedState);
#endif


			if (session->receivedMessage.messageStatus == Message_valid || session->receivedMessage.messageStatus == Message_repeated) {
				if ((*session->receivedMessage.type & 0xe0) == (TYPE_KEP1_SEND & 0xe0)) {
					session->state.kepState = kepContinueDrone(session, session->state.kepState);
				}
				else if ((*session->receivedMessage.type & 0xe0) == (TYPE_COMM_SEND & 0xe0)) {
					session->state.commState = messResContinue(session, &session->comm, session->state.commState);
					session->state.statState = messReqContinue(session, &session->stat, session->state.statState);
					session->state.feedState = messReqContinue(session, &session->feed, session->state.feedState);
					session->state.reqAliveState = messReqContinue(session, &session->aliveReq, session->state.reqAliveState);
					session->state.resAliveState = messResContinue(session, &session->aliveRes, session->state.resAliveState);
				}
				else if ((*session->receivedMessage.type & 0xe0) == (TYPE_STAT_SEND & 0xe0)) {
					session->state.statState = messReqContinue(session, &session->stat, session->state.statState);
					session->state.commState = messResContinue(session, &session->comm, session->state.commState);
					session->state.feedState = messReqContinue(session, &session->feed, session->state.feedState);
					session->state.reqAliveState = messReqContinue(session, &session->aliveReq, session->state.reqAliveState);
					session->state.resAliveState = messResContinue(session, &session->aliveRes, session->state.resAliveState);
				}
				else if ((*session->receivedMessage.type & 0xe0) == (TYPE_FEED_SEND & 0xe0)) {
					session->state.feedState = messReqContinue(session, &session->feed, session->state.feedState);
					session->state.commState = messResContinue(session, &session->comm, session->state.commState);
					session->state.statState = messReqContinue(session, &session->stat, session->state.statState);
					session->state.reqAliveState = messReqContinue(session, &session->aliveReq, session->state.reqAliveState);
					session->state.resAliveState = messResContinue(session, &session->aliveRes, session->state.resAliveState);
				}
				else if ((*session->receivedMessage.type & 0xe0) == (TYPE_ALIVE_SEND & 0xe0)) {
					/* Never call both simultaneously! */
					if (*session->receivedMessage.type == TYPE_ALIVE_SEND)
						session->state.resAliveState = messResContinue(session, &session->aliveRes, session->state.resAliveState);
					else
						session->state.reqAliveState = messReqContinue(session, &session->aliveReq, session->state.reqAliveState);
					
					session->state.commState = messResContinue(session, &session->comm, session->state.commState);
					session->state.statState = messReqContinue(session, &session->stat, session->state.statState);
					session->state.feedState = messReqContinue(session, &session->feed, session->state.feedState);
				} else {
					session->receivedMessage.messageStatus = Message_invalid;
				}
			}
			else {
				session->state.commState = messResContinue(session, &session->comm, session->state.commState);
				session->state.statState = messReqContinue(session, &session->stat, session->state.statState);
				session->state.feedState = messReqContinue(session, &session->feed, session->state.feedState);
				session->state.reqAliveState = messReqContinue(session, &session->aliveReq, session->state.reqAliveState);
				session->state.resAliveState = messResContinue(session, &session->aliveRes, session->state.resAliveState);
			}
		}
		else
			session->state.systemState = ClearSession;
		break;

	case ClearSession:
		/* Clear session and go to idle state */
		clearSessionDrone(session);
		printf("Session cleared!\n");
		sleep(1);
		break;

	default:
		session->state.systemState = ClearSession;
		break;
	}
}

void setExternalDroneCommands(struct externalCommands* external, uint8_t key) {
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

void loopDrone(struct SessionInfo* session, struct externalCommands* external) {
	uint8_t key;

	while (1) {
		/* Deal with external commands. */
		if (kbhit()) {
			key = readChar();
			setExternalDroneCommands(external, key);
		}
		else {
			setExternalDroneCommands(external, '\0');
		}

		/* Hand control to state machine */
		stateMachineDrone(session, external);
	}
}

int main_drone(int txPipe, int rxPipe) {
	struct SessionInfo session;
	struct externalCommands external;

	initializeDroneSession(&session, txPipe, rxPipe);
	setExternalDroneCommands(&external, '\0');

	loopDrone(&session, &external);

	return 0;
}

#if WINDOWS
int main_drone_win(struct threadParam* params) {
	struct SessionInfo session;
	struct externalCommands external;

	initializeDroneSession(&session, (int)params->txPipe, (int)params->rxPipe);
	setExternalDroneCommands(&external, '\0');

	loopDrone(&session, &external);
}
#endif