#include "params.h"
#include "session_info.h"
#include "crt_drbg.h"
#include "aegis.h"
#include "enc_dec.h"

#ifndef SESSION_SEND_SM_H_
#define SESSION_SEND_SM_H_

messState messSendContinue(struct SessionInfo* session, struct MESS_ctx* ctx, messState currentState);

#endif