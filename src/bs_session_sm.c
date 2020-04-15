#include "./../include/bs_session_sm.h"

statState statContinue(struct SessionInfo* session, statState currentState) {
	return STAT_idle;
}

commState commContinue(struct SessionInfo* session, commState currentState) {
	return COMM_idle;
}

feedState feedContinue(struct SessionInfo* session, feedState currentState) {
	return FEED_verify;
}