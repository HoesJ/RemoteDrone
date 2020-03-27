#include "params.h"
#include "session_info.h"
#include "pipe_io.h"
#include "crt_drbg.h"
#include "bs_kep_sm.h"

#ifndef MAIN_ASEE_STATION_H_
#define MAIN_BASE_STATION_H_

int main_base_station(int txPipe, int rxPipe);

#if WINDOWS
int main_base_station_win(struct threadParam* params);
#endif

#endif