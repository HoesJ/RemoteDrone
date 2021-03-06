#include "./../../include/sm/udp_io.h"

static const uint8_t FLAG = 0xAA;
static const uint8_t ESC  = 0xBB;

#if WINDOWS
int write(int pipe, const uint8_t* buffer, int nb) {
	struct pipe* p = (struct pipe*)pipe;
	int tmpWO = p->writeOffset;

	int spaceLeft = 2048 - (p->writeOffset - p->readOffset >= 0 ? p->writeOffset - p->readOffset : p->writeOffset + (2048 - p->readOffset));
	if (spaceLeft < nb)
		return -1;

	for (int i = 0; i < nb; i++) {
		p->buffer[tmpWO++] = buffer[i];
		if (tmpWO >= 2048)
			tmpWO = 0;
	}
	p->writeOffset = tmpWO;
	return nb;
}

int read(int pipe, uint8_t* buffer, int nb) {
	struct pipe* p = (struct pipe*)pipe;
	int tmpRO = p->readOffset;

	int toRead = (p->writeOffset - p->readOffset >= 0) ? p->writeOffset - p->readOffset : p->writeOffset + (2048 - p->readOffset);
	int i;
	for (i = 0; i < toRead && i < nb; i++) {
		buffer[i] = p->buffer[tmpRO++];
		if (tmpRO >= 2048)
			tmpRO = 0;
	}
	p->readOffset = tmpRO;
	return i;
}
#endif

/**
 * Initialize the given IO context. This function should always be
 * called before data is read from or written into its pipes.
 */
void init_IO_ctx(struct IO_ctx *IO, int txPipe, int rxPipe) {
	IO->txPipe = txPipe;
	IO->rxPipe = rxPipe;

	IO->bufferIndex = 0;
	IO->bufferSize = 0;

	IO->endOfMessage = 1;
	IO->escRead = 0;

	IO->resIndex = 0;
}

/**
 * Make sure that in the next call we start writing into the buffer at index
 * zero, even if cont is set to non-zero value.
 */
void resetCont_IO_ctx(struct IO_ctx *IO) {
	IO->resIndex = 0;
}

ssize_t writeWithErrors(int pipe, uint8_t* buffer, int length) {
	uint32_t berCount;
	uint32_t i, j;
	uint64_t rnd;
	uint8_t bk_buffer[MAX_MESSAGE_NB];

	memcpy(bk_buffer, buffer, length);

	berCount = 0;
	/* Modify data in buffer if not escape byte */
	if (bk_buffer != &ESC && bk_buffer != &FLAG) {
		for (i = 0; i < length; i++) {
			for (j = 0; j < sizeof(uint8_t) * 8; j++) {
				getRandomBytes(8, &rnd);
				if (rnd < FRAC_BER * 0xffffffffffffffff) {
					bk_buffer[i] = bk_buffer[i] & (1 << j);
					berCount++;
				}
			}
		}
		if (berCount != 0)
			printf("Written with %d errors\n", berCount);
	}

	/* Send through UDP or pipe */
#if UDP
	return send_message(bk_buffer, length);
#else
	return write(pipe, bk_buffer, length);
#endif
}

/**
 * Write output to pipe or UDP socket.
 */
ssize_t writeOut(int fd, const void *buf, size_t n) {
#if MAKE_BER
	return writeWithErrors(fd, buf, n);
#else
#if UDP
	return send_message((uint8_t*)buf, n);
#else
    return write(fd, buf, n);
#endif
#endif
}

/**
 * Send message to the other pipe. The parameter nbBytes gives the
 * number of bytes to send. A null termination must not be included.
 * Returns the number written or -1. If endOfMessage is set to non-zero,
 * the message will be terminated after it is sent through the pipe.
 */
ssize_t transmit(const struct IO_ctx *state, const void *buffer, size_t nbBytes, uint8_t endOfMessage) {
    size_t currentIndex;
    size_t nextSendIndex = 0;

    /* If FLAG or ESC character occurs in the byte steam, stuff one ESC character. */
    for (currentIndex = 0; currentIndex < nbBytes; currentIndex++) {
        if (((uint8_t*)buffer)[currentIndex] == FLAG || ((uint8_t*)buffer)[currentIndex] == ESC) {
            if (writeOut(state->txPipe, ((uint8_t*)buffer) + nextSendIndex, currentIndex - nextSendIndex) == -1)
                return -1;
            if (writeOut(state->txPipe, &ESC, 1) == -1)
                return -1;
            
            nextSendIndex = currentIndex;
        }
    }
    if (writeOut(state->txPipe, ((uint8_t*)buffer) + nextSendIndex, nbBytes - nextSendIndex) == -1)
        return -1;
    
    /* Write FLAG to indicate end of message. */
    if (endOfMessage && writeOut(state->txPipe, &FLAG, 1) == -1)
        return -1;

#if UDP
    /* Make sure that message is sent. */
    if (flush_buffer() == -1)
        return -1;
#endif
    return nbBytes;
}

/**
 * Read input from pipe or socket.
 */
ssize_t readIn(int fd, void *buf, size_t n) {
#if UDP
	return receive_message((uint8_t*)buf);
#else
	return read(fd, buf, n);
#endif
}

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
ssize_t receive(struct IO_ctx *state, void *result, size_t size, uint8_t cont) {
	size_t nbBytesWritten;
	size_t startCopyIndex = state->bufferIndex;

	/* If cont is zero, we start writing again at first index of result. */
	if (!cont)
		state->resIndex = 0;

	while (1) {
		/* Read from buffer while there are characters left. */
		for (; state->bufferIndex < state->bufferSize && state->resIndex + state->bufferIndex - startCopyIndex < size; state->bufferIndex++) {
			/* If we have read an escape character in the previous iteration, continue. */
			if (state->escRead) {
				state->escRead = 0;
				continue;
			}

			/* If we encounter an ESC, copy all info from the buffer to the result. */
			if (state->buffer[state->bufferIndex] == ESC) {
				state->escRead = 1;

				memcpy((uint8_t*)result + state->resIndex, state->buffer + startCopyIndex, state->bufferIndex - startCopyIndex);
				state->resIndex += (state->bufferIndex - startCopyIndex);
				startCopyIndex = state->bufferIndex + 1;
			}
			/* End of message if we encounter a FLAG. */
			else if (state->buffer[state->bufferIndex] == FLAG) {
				state->endOfMessage = 1;

				memcpy((uint8_t*)result + state->resIndex, state->buffer + startCopyIndex, state->bufferIndex - startCopyIndex);
				nbBytesWritten = state->resIndex + state->bufferIndex - startCopyIndex;

				state->bufferIndex++;
				state->resIndex = 0;
				return nbBytesWritten;
			}
		}

		/* Copy remaining bytes to result. */
		memcpy((uint8_t*)result + state->resIndex, state->buffer + startCopyIndex, state->bufferIndex - startCopyIndex);
		state->resIndex += (state->bufferIndex - startCopyIndex);

		/* Return if result is full. */
		if (state->resIndex == size) {
			state->resIndex = 0;
			state->endOfMessage = 0;
			return size;
		}

		/* Read new input from the pipe as long as it is non-empty. */
		startCopyIndex = 0;
		state->bufferIndex = 0;
		state->bufferSize = readIn(state->rxPipe, state->buffer, MAX_PACKET_SIZE);
		if (state->bufferSize == -1 || state->bufferSize == 0) {
			state->bufferSize = 0;
			state->endOfMessage = 0;
			return state->resIndex;
		}
	}
}