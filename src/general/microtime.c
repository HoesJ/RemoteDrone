#include "./../../include/general/microtime.h"

/**
 * Returns the current time in microseconds.
 */
uint64_t getMicrotime() {
#if UNIX
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	return currentTime.tv_sec * (uint64_t)1e6 + currentTime.tv_usec;
#endif

#if WINDOWS
	return (uint64_t)(clock() * 1e6)/CLOCKS_PER_SEC;
#endif	
}