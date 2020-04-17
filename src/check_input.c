#include "./../include/check_input.h"

/**
 * Check whether there is command input available. Return the number of
 * bytes written.
 */
ssize_t checkCommInput(uint8_t *buffer, size_t size) {
    #define COMM_LENGTH 14
    uint8_t text[COMM_LENGTH] = "Command sent!";
    uint8_t key;

    /* Check if input received. */
    if (!kbhit())
        return 0;
    
    key = readChar();
    if (key == '5') {
        memcpy(buffer, text, COMM_LENGTH);
        return COMM_LENGTH;
    } else if (key == '1' || key == '2' || key == '3' || key == '4') {
        ungetc(key, stdin);
        return 0;
    }
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
    while (count < size && (ch = fgetc(feed)) != EOF)
        buffer[count++] = ch;
    
    return count;
}

/**
 * Check whether there is stat input available. Return the number of
 * bytes written.
 */
ssize_t checkStatInput(uint8_t *buffer, size_t size) {
    #define STAT_LENGTH 15
    uint8_t text[STAT_LENGTH] = "Everything OK!";

    if (size < STAT_LENGTH)
        return -1;
    
    memcpy(buffer, text, STAT_LENGTH);
    return STAT_LENGTH;
}