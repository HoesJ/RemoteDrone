#include "params.h"
#include <sys/time.h>

#ifndef MICROTIME_H_
#define MICROTIME_H_

/**
 * Returns the current time in microseconds.
 */
uint64_t getMicrotime();

#endif