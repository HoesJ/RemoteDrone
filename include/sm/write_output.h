#include "./../general/params.h"

#ifndef WRITE_OUTPUT_H_
#define WRITE_OUTPUT_H_

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

#endif