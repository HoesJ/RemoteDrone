#include "./../../include/general/microtime.h"

/**
 * Returns the current time in microseconds.
 */
uint64_t getMicrotime() {
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	return currentTime.tv_sec * (uint64_t)1e6 + currentTime.tv_usec;
}