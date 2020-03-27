#include "./../include/pipe_io.h"

static const uint8_t FLAG = 0;
static const uint8_t ESC = 1;

/**
 * Initialize the given IO context. This function should always be
 * called before data is read from or written into its pipes.
 */
void init_IO_ctx(struct IO_ctx *IO, int txPipe, int rxPipe) {
    IO->txPipe = txPipe;
    IO->rxPipe = rxPipe;

    IO->bufferIndex  = 0;
    IO->bufferSize   = 0;

    IO->endOfMessage = 1;
    IO->escRead      = 0;
}

/**
 * Send message to the other pipe. The parameter nbBytes gives the
 * number of bytes to send. A null termination must not be included.
 * Returns the number written or -1. If endOfMessage is set to non-zero,
 * the message will be terminated after it is sent through the pipe.
 */
ssize_t send(const struct IO_ctx *state, const void *buffer, size_t nbBytes, word endOfMessage) {
    size_t currentIndex;
    size_t nextSendIndex = 0;

    /* If FLAG or ESC character occurs in the byte steam, stuff one ESC character. */
    for (currentIndex = 0; currentIndex < nbBytes; currentIndex++) {
        if (((uint8_t*)buffer)[currentIndex] == FLAG || ((uint8_t*)buffer)[currentIndex] == ESC) {
            if (write(state->txPipe, ((uint8_t*)buffer) + nextSendIndex, currentIndex - nextSendIndex) == -1)
                return -1;
            if (write(state->txPipe, &ESC, 1) == -1)
                return -1;
            
            nextSendIndex = currentIndex;
        }
    }
    if (write(state->txPipe, ((uint8_t*)buffer) + nextSendIndex, nbBytes - nextSendIndex) == -1)
        return -1;
    
    /* Write flag to indicate end of message. */
    if (endOfMessage && write(state->txPipe, &FLAG, 1) == -1)
        return -1;

    return nbBytes;
}

/**
 * Receive message from the other pipe. The parameter nbBytes gives the
 * number of bytes that can be read into the buffer. A null termination
 * will not be included. Returns the number of bytes written into the
 * buffer (possibly 0) or -1. If the current message might still have
 * bytes left in the pipe, the value of state->endOfMessage is set to zero.
 * Else, the value of state->endOfMessage is set to non-zero. If cont is
 * set to non-zero, the function will continue writing to result at the
 * index where it stopped last time. This will only happen if the message
 * was not yet finished and result was not yet full.
 * 
 * Note that this function will not necessarily set state->endOfMessage
 * to non-zero if there are no bytes left in the pipe.
 */
ssize_t receive(struct IO_ctx *state, void *result, size_t size, word cont) {
    size_t nbBytesWritten = 0;
    size_t startCopyIndex = state->bufferIndex;
    
    /* If cont is zero, we start writing again at first index of result. */
    if (!cont)
        state->resIndex = 0;

    while (1) {
        /* Read from buffer while there are characters left. */
        for (; state->bufferIndex < state->bufferSize, state->resIndex + state->bufferIndex - startCopyIndex < size; state->bufferIndex++) {
            /* If we have read an escape character in the previous iteration, continue. */
            if (state->escRead) {
                state->escRead = 0;
                nbBytesWritten++;
                continue;
            }
            
            /* If we encounter an escape character, copy all info from the buffer to the result. */
            if (state->buffer[state->bufferIndex] == ESC) {
                state->escRead = 1;

                memcpy((uint8_t*)result + state->resIndex, state->buffer, state->bufferIndex - startCopyIndex);
                state->resIndex += (state->bufferIndex - startCopyIndex);
                startCopyIndex = state->bufferIndex + 1;
            } 
            /* End of message if we encounter a flag. */
            else if (state->buffer[state->bufferIndex] == FLAG) {
                state->endOfMessage = 1;

                memcpy((uint8_t*)result + state->resIndex, state->buffer, state->bufferIndex - startCopyIndex);

                state->bufferIndex++;
                state->resIndex = 0;
                return;
            } else {
                nbBytesWritten++;
            }
        }

        /* Copy remaining bytes to result. */
        memcpy((uint8_t*)result + state->resIndex, state->buffer, state->bufferIndex - startCopyIndex);
        state->resIndex += (state->bufferIndex - startCopyIndex);

        /* Return if result is full. */
        if (state->resIndex == size) {
            state->resIndex = 0;
            state->endOfMessage = 0;
            return nbBytesWritten;
        }

        /* Read new input from the pipe as long as it is non-empty. */
        state->bufferIndex = 0;
        state->bufferSize = read(state->rxPipe, state->buffer, BUFFER_SIZE);
        if (state->bufferSize == -1) {
            state->bufferSize   = 0;
            state->endOfMessage = 0;
            return -1;
        } else if (state->bufferSize == 0) {
            state->endOfMessage = 0;
            return nbBytesWritten;
        }
    }
}