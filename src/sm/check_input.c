#include "./../../include/sm/check_input.h"

/* Initialize global flags */
uint8_t FEED_ACTIVE = 0;
uint8_t STAT_ACTIVE = 0;

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