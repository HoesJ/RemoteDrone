#include "params.h"
#include "session_info.h"
#include "pipe_io.h"
#include "enc_dec.h"
#include "drone_kep_sm.h"
#include "session_init_sm.h"
#include "session_req_sm.h"
#include "session_res_sm.h"

#ifndef MAIN_DONE_H_
#define MAIN_DRONE_H_

int main_drone(int txPipe, int rxPipe);

#if WINDOWS
int main_drone_win(struct threadParam* params);
#endif

#endif