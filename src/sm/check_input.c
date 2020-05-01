#include "./../../include/sm/check_input.h"

/**
 * Check whether there is command input available. Return the number of
 * bytes written.
 */
size_t checkCommInput(uint8_t *buffer, size_t size) {
    #define COMM_LENGTH 15
    uint8_t text[COMM_LENGTH] = "Command sent!\n";

    /* Check if input received. */
    if (size < COMM_LENGTH || !kbhit())
        return 0;
    
    if (readChar() == 'c') {
        memcpy(buffer, text, COMM_LENGTH);
        return COMM_LENGTH;
    } else {
        return 0;
    }
}

/**
 * Check whether there is feed input available. Return the number of
 * bytes written.
 */
size_t checkStatInput(uint8_t *buffer, size_t size) {
    static uint8_t feedOpen = 0, feedClosed = 0;
    static FILE *feed;
    int ch;
    size_t count;

    /* If feed was closed already, no bytes are read. */
    if (feedClosed)
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
	printf("Count: %d\n", count);

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
size_t checkFeedInput(uint8_t *buffer, size_t size) {
    #define STAT_LENGTH 16
    uint8_t text[STAT_LENGTH] = "Everything OK!\n";
    static uint8_t statusSent = 0;

    if (statusSent || size < STAT_LENGTH)
        return 0;
    
    statusSent = 1;
    memcpy(buffer, text, STAT_LENGTH);
    return STAT_LENGTH;
}