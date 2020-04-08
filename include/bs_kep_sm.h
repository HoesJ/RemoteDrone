#include "params.h"
#include "session_info.h"
#include "ecdh.h"
#include "ecdsa.h"
#include "ec_arithmetic.h"
#include "aegis.h"
#include "pipe_io.h"
#include "enc_dec.h"

#ifndef BS_KEP_SM_H_
#define BS_KEP_SM_H_

void init_KEP_ctxBaseStation(struct KEP_ctx* ctx);

kepState kepContinueBaseStation(struct SessionInfo* session, kepState currentState);

#endif