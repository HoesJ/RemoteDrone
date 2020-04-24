#include "./../general/params.h"
#include "./../sm/session_info.h"
#include "./../sm/pipe_io.h"
#include "./../sm/enc_dec.h"
#include "./../sm/drone_kep_sm.h"
#include "./../sm/session_init_sm.h"
#include "./../sm/session_req_sm.h"
#include "./../sm/session_res_sm.h"

#ifndef MAIN_DONE_H_
#define MAIN_DRONE_H_

int main_drone(int txPipe, int rxPipe);

#if WINDOWS
int main_drone_win(struct threadParam* params);
#endif

#endif