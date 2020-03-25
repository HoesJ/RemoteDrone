#include "./../include/pipe_io.h"

static const uint8_t FLAG = 0;
static const uint8_t ESC = 1;

#define BUFFER_SIZE 128;

/**
 * Send message to the other pipe. The parameter nbBytes gives the
 * number of bytes to send. A null termination must not be included.
 * Returns the number written or -1.
 */
ssize_t send(const struct STATE_ctx *state, const void *buffer, size_t nbBytes) {
    size_t currentIndex;
    size_t nextSendIndex = 0;

    /* If FLAG or ESC character occurs in the byte steam, stuff one ESC character. */
    for (currentIndex = 0; currentIndex < nbBytes; currentIndex++) {
        if (((uint8_t*)buffer)[currentIndex] == FLAG || ((uint8_t*)buffer)[currentIndex] == ESC) {
            if (write(state->txPipe[1], ((uint8_t*)buffer) + nextSendIndex, currentIndex - nextSendIndex) == -1)
                return -1;
            if (write(state->txPipe[1], &ESC, 1) == -1)
                return -1;
            
            nextSendIndex = currentIndex;
        }
    }
    if (write(state->txPipe[1], ((uint8_t*)buffer) + nextSendIndex, nbBytes - nextSendIndex) == -1)
        return -1;
    
    /* Write flag to indicate end of message. */
    if (write(state->txPipe[1], &FLAG, 1) == -1)
        return -1;

    return nbBytes;
}

/**
 * Receive message from the other pipe. The parameter nbBytes gives the
 * number of bytes that can be read into the buffer. A null termination
 * will not be included. Returns the number of bytes written into the
 * buffer (possibly 0) or -1. If the current message still has bytes
 * left in the pipe, the value of state->endOfMessage is set to zero.
 * Else, the value of state->endOfMessage is set to non-zero.
 */
ssize_t receive(const struct STATE_ctx *state, const void *buffer, size_t size) {
    uint8_t buffer[BUFFER_SIZE]; /* Should be in ctx. */
    ssize_t nbBytesRead;
    word escRead = 0;

    nbBytesRead = read(state->rxPipe[0], buffer, BUFFER_SIZE);
    if (nbBytesRead == -1) {
        return -1;
    } else {
        for (currentIndex = 0; currentIndex < nbBytesRead; currentIndex++) {
            if (buffer[currentIndex] == ESC) {
                escRead = 1;
            } else if (buffer[currentIndex] = )
            if (write(state->txPipe[1], &ESC, 1) == -1)
                return -1;
            
            nextSendIndex = currentIndex;
        }
    }
}