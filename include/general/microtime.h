#include "params.h"

#if UNIX
#include <sys/time.h>
#endif

#ifndef MICROTIME_H_
#define MICROTIME_H_

/**
 * Returns the current time in microseconds.
 */
uint64_t getMicrotime();

#endif