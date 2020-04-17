#include "./../include/main_drone.h"
#include "./../include/drone_kep_sm.h"

void initializeDroneSession(struct SessionInfo* session, int txPipe, int rxPipe) {
	/* Initialize state */
	session->state.systemState = Idle;
	session->state.kepState = KEP_idle;
	session->state.commState = MESS_idle;
	session->state.statState = MESS_idle;
	session->state.feedState = MESS_idle;

	/* Initialize IO ctx */
	init_IO_ctx(&session->IO, txPipe, rxPipe);

	/* Initialize KEP ctx */
	init_KEP_ctxDrone(&session->kep);

	/* Initialize session key */

	/* Initialize sequence NB */
	getRandomBytes(sizeof(word), &session->sequenceNb);

	/* Initialize expected sequence NB */
	session->expectedSequenceNb = 0;

	/* Initialize target ID */
	memset(session->targetID, 0, FIELD_TARGET_NB);

	/* Initialize own target ID */
	memset(session->ownID, 0, FIELD_TARGET_NB);
	session->ownID[0] = 1;

	/* Initialize message status. */
	session->receivedMessage.messageStatus = Channel_empty;
}

void clearSessionDrone(struct SessionInfo* session) {
	/* Re-Initialize state */
	session->state.systemState = Idle;
	session->state.kepState = KEP_idle;
	session->state.commState = MESS_idle;
	session->state.statState = MESS_idle;
	session->state.feedState = MESS_idle;

	/* Re-Initialize KEP ctx */
	init_KEP_ctxDrone(&session->kep);

	/* Re-Initialize sequence NB */
	getRandomBytes(sizeof(word), &session->sequenceNb);

	/* Re-Initialize expected sequence NB */
	session->expectedSequenceNb = 0;
}

void stateMachineDrone(struct SessionInfo* session, struct externalBaseStationCommands* external) {
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
			if (session->state.kepState == KEP2_wait_request || session->state.kepState == KEP2_wait || session->state.kepState == KEP4_wait) {
				pollAndDecode(session);
			}
			else
				printf("Drone\t- current state: %d\n", session->state.kepState);

			/* Sets ClearSession if something goes wrong */
			session->state.kepState = kepContinueDrone(session, session->state.kepState);

			/* If KEP is done, go to next state */
			if (session->state.kepState == Done) {
				session->state.systemState = SessionReady;
				printf("Drone\t- session ready\n");
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
		break;

	case ClearSession:
		/* Clear session and go to idle state */
		clearSessionDrone(session);
		break;

	default:
		session->state.systemState = ClearSession;
		break;
	}
}

void setExternalDroneCommands(struct externalDroneCommands* external, uint8_t key) {
	switch (key) {
	case 's':
		external->start = 1;
		external->quit = 0;
		external->feedCommand = 0;
		external->updateCommand = 0;
		break;
	case 'q':
		external->start = 0;
		external->quit = 1;
		external->feedCommand = 0;
		external->updateCommand = 0;
		break;
	case 'f':
		external->start = 0;
		external->quit = 0;
		external->feedCommand = 1;
		external->updateCommand = 0;
		break;
	case 'u':
		external->start = 0;
		external->quit = 0;
		external->feedCommand = 0;
		external->updateCommand = 1;
		break;
	default:
		external->start = 0;
		external->quit = 0;
		external->feedCommand = 0;
		external->updateCommand = 0;
		ungetc(key, stdin);
		break;
	}
}

void loopDrone(struct SessionInfo* session, struct externalDroneCommands* external) {
	uint8_t key, receivedType;
	uint8_t command[256];
	word	externalOn;

	externalOn = 0;
	while (1) {
		/* Deal with external commands. For now, these will be given as
		   keyboard inputs. */
		if (kbhit()) {
			key = readChar();
			setExternalDroneCommands(external, key);
			externalOn = 1;
		} else if (externalOn) {
			/* Clear external signals */
			setExternalDroneCommands(external, '\0');
			externalOn = 0;
		}

		/* Hand control to state machine */
		stateMachineDrone(session, external);
	}
}

int main_drone(int txPipe, int rxPipe) {
	struct SessionInfo session;
	struct externalDroneCommands external;

	initializeDroneSession(&session, txPipe, rxPipe);
	setExternalDroneCommands(&external, '\0');

	loopDrone(&session, &external);

	return 0;
}

#if WINDOWS
#include "./../include/enc_dec.h"
#include "./../include/print_number.h"
int main_drone_win(struct threadParam* params) {
	struct SessionInfo session;
	struct externalDroneCommands external;

	initializeDroneSession(&session, (int)params->txPipe, (int)params->rxPipe);
	setExternalDroneCommands(&external, '\0');

	loopDrone(&session, &external);
}
#endif