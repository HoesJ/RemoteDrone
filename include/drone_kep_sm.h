#include "params.h"
#include "session_info.h"
#include "ecdh.h"
#include "ecdsa.h"
#include "ec_arithmetic.h"
#include "aegis.h"
#include "pipe_io.h"

#ifndef DRONE_KEP_SM_H_
#define DRONE_KEP_SM_H_

void init_KEP_ctxDrone(struct KEP_ctx* ctx);

kepState kepContinueDrone(struct SessionInfo* session, kepState currentState);

#endif