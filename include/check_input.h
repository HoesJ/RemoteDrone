#include "params.h"
#include "read_char.h"

#ifndef CHECK_INPUT_H_
#define CHECK_INPUT_H_

/**
 * Check whether there is command input available. Return the number of
 * bytes written.
 */
ssize_t checkCommInput(uint8_t *buffer, size_t size);

/**
 * Check whether there is feed input available. Return the number of
 * bytes written.
 */
ssize_t checkFeedInput(uint8_t *buffer, size_t size);

/**
 * Check whether there is stat input available. Return the number of
 * bytes written.
 */
ssize_t checkStatInput(uint8_t *buffer, size_t size);

#endif