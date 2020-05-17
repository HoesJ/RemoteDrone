#include "./../../include/general/microtime.h"

/**
 * Returns the current time in microseconds.
 */
uint64_t getMicrotime() {
	return (uint64_t)(clock() * 1e6) / CLOCKS_PER_SEC;
}