#include "params.h"
#include "session_info.h"
#include "pipe_io.h"

#ifndef MAIN_DONE_H_
#define MAIN_DRONE_H_

int main_drone(int txPipe, int rxPipe);

#if WINDOWS
int main_drone_win(struct threadParam* params);
#endif


#endif