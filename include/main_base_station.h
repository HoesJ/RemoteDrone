#include "params.h"
#include "session_info.h"
#include "crt_drbg.h"
#include "bs_kep_sm.h"
#include "enc_dec.h"
#include "read_char.h"
#include "bs_kep_sm.h"
#include "session_init_sm.h"
#include "session_req_sm.h"
#include "session_res_sm.h"

#ifndef MAIN_ASEE_STATION_H_
#define MAIN_BASE_STATION_H_

int main_base_station(int txPipe, int rxPipe);

#if WINDOWS
int main_base_station_win(struct threadParam* params);
#endif

#endif