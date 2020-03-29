#include "./../include/main_base_station.h"

void initializeSession(struct SessionInfo* session, int txPipe, int rxPipe) {
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

	/* Initialize target ID */
	session->targetID = 1;
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

void loop()
{
}

void stateMachine(struct SessionInfo* session, struct externalBaseStationCommands* external) {
	switch (session->state.systemState)
	{
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

int main_base_station(int txPipe, int rxPipe) {
	struct IO_ctx state;
	char buffer[26] = "Message from base station";
	ssize_t length;

	init_IO_ctx(&state, txPipe, rxPipe);

	/* Send test message */
	printf("BS - sending text message\n");
	length = transmit(&state, buffer, 25, 1);

	printf("BS - Done!\n");

	return 0;
}

#if WINDOWS
int main_base_station_win(struct threadParam *params) {
	struct IO_ctx state;
	char buffer[26] = "Message from base station";
	ssize_t length;

	init_IO_ctx(&state, (int)params->txPipe, (int)params->rxPipe);

	/* Send test message */
	printf("BS -\tsending text message\n");
	length = transmit(&state, buffer, 25, 1);
	buffer[0] = 'Q';
	length = transmit(&state, buffer, 25, 1);

	printf("BS -\tDone!\n");

	return 0;
}
#endif