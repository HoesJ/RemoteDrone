#include "./../include/check_input.h"

/**
 * Check whether there is command input available. Return the number of
 * bytes written.
 */
size_t checkCommInput(uint8_t *buffer, size_t size) {
    #define COMM_LENGTH 14
    uint8_t text[COMM_LENGTH] = "Command sent!";

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
size_t checkFeedInput(uint8_t *buffer, size_t size) {
    static uint8_t feedOpen = 0;
    static FILE *feed;
    char ch;
    size_t count;

	return 0;

    /* Open feed if necessary. */
    if (!feedOpen) {
        feed = fopen("./feed.txt", "r");
        feedOpen = 1;

        if (feed == NULL) {
            feedOpen = 0;
            printf("Error while opening the video feed.\n");
            return 0;
        }
    }

    /* Read bytes from feed. */
	count = 0;
    while (count < size && (ch = fgetc(feed)) != EOF)
        buffer[count++] = ch;
    
    return count;
}

/**
 * Check whether there is stat input available. Return the number of
 * bytes written.
 */
size_t checkStatInput(uint8_t *buffer, size_t size) {
    #define STAT_LENGTH 15

	return 0;

    uint8_t text[STAT_LENGTH] = "Everything OK!";
    static uint8_t statusSent = 0;

    if (statusSent || size < STAT_LENGTH)
        return 0;
    
    memcpy(buffer, text, STAT_LENGTH);
    statusSent = 1;
    return STAT_LENGTH;
}