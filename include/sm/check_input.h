#include "./../general/params.h"
#include "./../general/read_char.h"

#ifndef CHECK_INPUT_H_
#define CHECK_INPUT_H_

/**
 * Check whether there is command input available. Return the number of
 * bytes written.
 */
size_t checkCommInput(uint8_t *buffer, size_t size);

/**
 * Check whether there is feed input available. Return the number of
 * bytes written.
 */
size_t checkFeedInput(uint8_t *buffer, size_t size);

/**
 * Check whether there is stat input available. Return the number of
 * bytes written.
 */
size_t checkStatInput(uint8_t *buffer, size_t size);

/**
 * Flags for passing the command from input to write 
 */
extern uint8_t FEED_ACTIVE;
extern uint8_t STAT_ACTIVE;

#endif