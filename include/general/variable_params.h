#ifndef VARIABLE_PARAMS_H_
#define VARIABLE_PARAMS_H_

#ifdef  DEBUG
#define DEST_IP	    "127.0.0.1"
#define UDP 		0
#else
#ifndef RUN_OVERRIDE
#define DEST_IP	    "127.0.0.1" /* "94.226.244.66" */
#else
#ifdef DRONE
#define DEST_IP     "10.87.20.93" /* "10.87.20.96" */
#endif
#ifdef BS
#define DEST_IP     "10.87.20.96" /* "10.87.20.93" */
#endif
#endif
#define UDP 		1
#endif
#ifndef RUN_OVERRIDE
#define RUN_DRONE 	1
#define RUN_BS 		1
#else
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
#endif
#define FEED_INPUT  "./rsa1.mp4"
#define FEED_OUTPUT "./rec_feed.mp4"

#endif