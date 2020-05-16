#include "./../general/params.h"
#include "./../general/read_char.h"
#include "./../general/microtime.h"
#include "./../sm/udp.h"

#ifndef CHECK_INPUT_H_
#define CHECK_INPUT_H_

extern uint8_t feedClosed;
void openFeed();
void closeFeed();

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
 * Check whether there is alive input available. Return the number of
 * bytes written.
 */
size_t checkAliveInput(uint8_t *buffer, size_t size);

/**
 * Flags for passing the command from input to write 
 */
extern uint8_t FEED_ACTIVE;
extern uint8_t STAT_ACTIVE;

/**
 * Indicate when last we heard from other party
 */
extern uint64_t LAST_CHECK;
extern uint64_t MICRO_INTERVAL;

#endif