#include "./../include/session_send_sm.h"

messState messSendContinue(struct SessionInfo* session, messState currentState) {
	word input;

	switch (currentState)
	{
	case MESS_idle:
		//input = checkForInput();
		return input ? MESS_encrypt : MESS_idle;
	default:
		break;
	}
}