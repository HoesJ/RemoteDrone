#include "./../crypto/crt_drbg.h"
#include "./../general/params.h"
#include "./../general/microtime.h"
#include "./../sm/enc_dec.h"
#include "session_info.h"

#ifndef SESSION_RES_H_
#define SESSION_RES_H_

messState messResContinue(struct SessionInfo* session, struct MESS_ctx* ctx, messState currentState);

#endif