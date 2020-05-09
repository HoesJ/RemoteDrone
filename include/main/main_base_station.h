#include "./../crypto/crt_drbg.h"
#include "./../general/params.h"
#include "./../general/read_char.h"
#include "./../sm/session_info.h"
#include "./../sm/bs_kep_sm.h"
#include "./../sm/enc_dec.h"
#include "./../sm/bs_kep_sm.h"
#include "./../sm/session_init_sm.h"
#include "./../sm/session_req_sm.h"
#include "./../sm/session_res_sm.h"
#include "./../sm/check_input.h"

#ifndef MAIN_ASEE_STATION_H_
#define MAIN_BASE_STATION_H_

int main_base_station(int txPipe, int rxPipe);

#if WINDOWS
int main_base_station_win(struct threadParam* params);
#endif

#endif