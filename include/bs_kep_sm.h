#include "params.h"
#include "ecdh.h"
#include "ecdsa.h"
#include "ec_arithmetic.h"
#include "aegis.h"
#include "session_info.h"

#ifndef BS_KEP_SM_H_
#define BS_KEP_SM_H_

void init_KEP_ctx(struct KEP_ctx* ctx);

kepState kepContinue(struct SessionInfo* session, kepState currentState);

#endif