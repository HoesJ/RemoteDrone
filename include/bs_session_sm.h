#include "params.h"
#include "session_info.h"
#include "aegis.h"

#ifndef BS_SESSION_SM_H_
#define BS_SESSION_SM_H_

stateState stateContinue(struct SessionInfo* session, stateState currentState);

commState commContinue(struct SessionInfo* session, commState currentState);

feedState feedContinue(struct SessionInfo* session, feedState currentState);

#endif