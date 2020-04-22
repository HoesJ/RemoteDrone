#include "./../crypto/crt_drbg.h"
#include "./../crypto/aegis.h"
#include "./../general/params.h"
#include "session_info.h"
#include "enc_dec.h"

#ifndef SESSION_REQ_SM_H_
#define SESSION_REQ_SM_H_

messState messReqContinue(struct SessionInfo* session, struct MESS_ctx* ctx, messState currentState);

#endif