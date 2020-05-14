#include "./../crypto/ecdh.h"
#include "./../crypto/ecdsa.h"
#include "./../crypto/ec_arithmetic.h"
#include "./../crypto/aegis.h"
#include "./../general/params.h"
#include "./../general/microtime.h"
#include "./../sm/session_info.h"
#include "./../sm/udp_io.h"
#include "./../sm/enc_dec.h"

#ifndef BS_KEP_SM_H_
#define BS_KEP_SM_H_

void init_KEP_ctxBaseStation(struct KEP_ctx* ctx);

kepState kepContinueBaseStation(struct SessionInfo* session, kepState currentState);

#endif