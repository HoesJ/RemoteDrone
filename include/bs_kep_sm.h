#include "params.h"
#include "ecdh.h"
#include "ecdsa.h"
#include "aegis.h"

#ifndef BS_KEP_SM_H_
#define BS_KEP_SM_H_

typedef enum {
    Idle,
    KEP1_compute, KEP1_send, KEP1_wait, 
    KEP3_verify, KEP3_compute, KEP3_send, KEP3_wait,
    KEP5_verify
} kepState;


kepState kepContinue(kepState currentState);

#endif