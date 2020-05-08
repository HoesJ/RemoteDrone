#include "./../../include/sm/check_input.h"

/* Initialize global flags */
uint8_t FEED_ACTIVE = 0;
uint8_t STAT_ACTIVE = 0;
uint64_t LAST_CHECK = 0;
uint64_t MICRO_INTERVAL = 0;

/**
 * Check whether there is command input available. Return the number of
 * bytes written.
 */
size_t checkCommInput(uint8_t *buffer, size_t size) {
	uint8_t commandStat[] = COMMAND_STAT;
	uint8_t commandStartFeed[] = COMMAND_START_FEED;
	uint8_t commandStopFeed[] = COMMAND_STOP_FEED;
	uint8_t fromKeyboard;

    /* Check if input received. */
    if (size < COMMAND_START_FEED_LN || !kbhit()) {
        return 0;
    }
    
	fromKeyboard = readChar();
	switch (fromKeyboard) {
	case 'c':
		memcpy(buffer, commandStat, COMMAND_STAT_LN);
		return COMMAND_STAT_LN;
	case 'f':
		memcpy(buffer, commandStartFeed, COMMAND_START_FEED_LN);
		return COMMAND_START_FEED_LN;
	case 'g':
		memcpy(buffer, commandStopFeed, COMMAND_STOP_FEED_LN);
		return COMMAND_STOP_FEED_LN;
	default:
		return 0;
	}
}

#if LIVE_FEED_PORT
/**
 * Check whether there is feed input available. Return the number of
 * bytes written.
 */
size_t checkFeedInput(uint8_t* buffer, size_t size) {
	int received;

	if (!FEED_ACTIVE)
		return 0;

	received = receive_feed(buffer);
	return received == -1 ? 0 : received;
}

#else
/**
 * Check whether there is feed input available. Return the number of
 * bytes written.
 */
size_t checkFeedInput(uint8_t *buffer, size_t size) {
    static uint8_t feedOpen = 0, feedClosed = 0;
    static FILE *feed;
    size_t count;

    /* If feed was closed already, no bytes are read. */
    if (feedClosed || !FEED_ACTIVE)
        return 0;

    /* Open feed if necessary. */
    if (!feedOpen) {
        feed = fopen(FEED_INPUT, "rb");
        feedOpen = 1;

        if (feed == NULL) {
            feedOpen = 0;
            printf("Error while opening the video feed.\n");
            return 0;
        }
    }

    /* Read bytes from feed. */
	count = fread(buffer, 1, size, feed);

    /* Close file if no bytes left. */
    if (count != size) {
        feedClosed = 1;
        fclose(feed);
    }
    
    return count;
}
#endif

/**
 * Check whether there is stat input available. Return the number of
 * bytes written.
 */
size_t checkStatInput(uint8_t *buffer, size_t size) {
    #define STAT_LENGTH 15
    uint8_t text[STAT_LENGTH] = "Everything OK!";

    if (size < STAT_LENGTH || !STAT_ACTIVE) {
        return 0;
    }
    
	STAT_ACTIVE = 0;
    memcpy(buffer, text, STAT_LENGTH);
    return STAT_LENGTH;
}

/**
 * Check whether there is alive input available. Return the number of
 * bytes written.
 */
size_t checkAliveInput(uint8_t *buffer, size_t size) {
    uint64_t currentTime;

    if (size == 0)
        return 0;

    /* Send one dummy byte if too long no traffic. */
    currentTime = getMicrotime();
    if (currentTime - LAST_CHECK > MICRO_INTERVAL) {
        LAST_CHECK = currentTime;
        buffer[0] = 0;
        return 1;
    } else
        return 0;
}