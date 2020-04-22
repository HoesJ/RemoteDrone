#include "params.h"
#include "session_info.h"
#include "check_input.h"
#include "write_output.h"

#ifndef SESSION_INIT_SM_H_
#define SESSION_INIT_SM_H_

void init_COMM_ctx(struct MESS_ctx* ctx);

void init_STAT_ctx(struct MESS_ctx* ctx);

void init_FEED_ctx(struct MESS_ctx* ctx);


#endif
