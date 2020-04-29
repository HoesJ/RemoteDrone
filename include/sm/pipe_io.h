#include "./../general/params.h"
#include "session_info.h"
#include "udp.h"

#ifndef PIPE_IO_H_
#define PIPE_IO_H_

/**
 * Initialize the given IO context. This function should always be
 * called before data is read from or written into its pipes.
 */
void init_IO_ctx(struct IO_ctx *IO, int txPipe, int rxPipe);

/**
 * Make sure that in the next call we start writing into the buffer at index
 * zero, even if cont is set to non-zero value.
 */
void resetCont_IO_ctx(struct IO_ctx *IO);

/**
 * Send message to the other pipe. The parameter nbBytes gives the
 * number of bytes to send. A null termination must not be included.
 * Returns the number written or -1. If endOfMessage is set to non-zero,
 * the message will be terminated after it is sent through the pipe.
 */
ssize_t transmit(const struct IO_ctx *state, const void *buffer, size_t nbBytes, uint8_t endOfMessage);

/**
 * Receive message from the other pipe. The parameter nbBytes gives the
 * number of bytes that can be read into the buffer. A null termination
 * will not be included. Returns the number of bytes written into the
 * buffer (possibly 0 and including the previous function calls).
 * If the current message might still have bytes left in the pipe, the
 * value of state->endOfMessage is set to zero. Else, the value of
 * state->endOfMessage is set to non-zero. If cont is set to non-zero,
 * the function will continue writing to result at the index where it
 * stopped last time. This will only happen if the message was not yet
 * finished and the result was not yet full.
 * 
 * Note that this function will not necessarily set state->endOfMessage
 * to non-zero if there are no bytes left in the pipe.
 */
ssize_t receive(struct IO_ctx *state, void *result, size_t size, uint8_t cont);

#endif