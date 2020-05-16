#include "./../general/params.h"
#include "check_input.h"
#include "./../general/compare_arrays.h"
#include "udp.h"

#ifndef WRITE_OUTPUT_H_
#define WRITE_OUTPUT_H_

extern uint8_t rem;

/**
 * Write the output of command message.
 */
void writeCommOutput(uint8_t *buffer, size_t size);

/**
 * Write the output of video feed message.
 */
void writeFeedOutput(uint8_t *buffer, size_t size);

/**
 * Write the output of status update message.
 */
void writeStatOutput(uint8_t *buffer, size_t size);

/**
 * Write the output of alive message.
 */
void writeAliveOutput(uint8_t *buffer, size_t size);

#endif