#include "./../crypto/ecdh.h"
#include "./../crypto/ecdsa.h"
#include "./../crypto/ec_arithmetic.h"
#include "./../crypto/aegis.h"
#include "./../general/params.h"
#include "./../sm/session_info.h"
#include "./../sm/pipe_io.h"
#include "./../sm/enc_dec.h"

#ifndef DRONE_KEP_SM_H_
#define DRONE_KEP_SM_H_

void init_KEP_ctxDrone(struct KEP_ctx* ctx);

kepState kepContinueDrone(struct SessionInfo* session, kepState currentState);

#endif