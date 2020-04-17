#include "./../include/check_input.h"

/**
 * Check whether there is command input available. Return the number of
 * bytes written.
 */
ssize_t checkCommInput(uint8_t *buffer, size_t size) {
    #define COMM_LENGTH 34
    char text[COMM_LENGTH] = "Received the following command: \0";

    /* Check if input received and buffer is large enough. */
    if (!kbhit() || size < COMM_LENGTH)
        return -1;
    else
        text[COMM_LENGTH - 2] = getch();
    
    /* Copy result to buffer. */
    memcpy(buffer, text, COMM_LENGTH);
    return COMM_LENGTH;
}

/**
 * Check whether there is feed input available. Return the number of
 * bytes written.
 */
ssize_t checkFeedInput(uint8_t *buffer, size_t size) {
    static uint8_t feedOpen = 0;
    static FILE *feed;
    char ch;
    size_t count;

    /* Open feed if necessary. */
    if (!feedOpen) {
        feed = fopen("feed.txt", "r");
        feedOpen = 1;

        if (feed == NULL) {
            feedOpen = 0;
            printf("Error while opening the feed.\n");
            return -1;
        }
    }

    /* Read bytes from feed. */
    while ((ch = fgetc(feed)) != EOF && count < size)
        buffer[count++] = ch;
    
    return count;
}

/**
 * Check whether there is stat input available. Return the number of
 * bytes written.
 */
ssize_t checkStatInput(uint8_t *buffer, size_t size) {
    #define STAT_LENGTH 15
    char text[STAT_LENGTH] = "Everything OK!";

    if (size < STAT_LENGTH)
        return -1;
    
    memcpy(buffer, text, STAT_LENGTH);
    return STAT_LENGTH;
}