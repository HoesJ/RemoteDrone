#ifndef VARIABLE_PARAMS_H_
#define VARIABLE_PARAMS_H_

#ifdef DRONE
#define RUN_DRONE   1
#else
#define RUN_DRONE   0
#endif
#ifdef BS
#define RUN_BS      1
#else
#define RUN_BS      0
#endif

#define FEED_INPUT  "./feed.jpg"
#define FEED_OUTPUT "./rec_feed.jpg"

#endif