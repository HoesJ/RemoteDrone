#include "params.h"
#include "session_info.h"
#include "crt_drbg.h"
#include "aegis.h"
#include "enc_dec.h"

#ifndef SESSION_REQ_SM_H_
#define SESSION_REQ_SM_H_

messState messReqContinue(struct SessionInfo* session, struct MESS_ctx ctx, messState currentState);

#endif