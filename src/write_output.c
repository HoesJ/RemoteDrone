#include "./../include/write_output.h"

/**
 * Write the output of command message.
 */
void writeCommOutput(uint8_t *buffer, size_t size) {
    printf("Received command: ");
    printf((const char*)buffer);
}

/**
 * Write the output of video feed message.
 */
void writeFeedOutput(uint8_t *buffer, size_t size) {
    FILE *feed;

    /* Open feed. */
    feed = fopen("./rec_feed.txt", "a");

    if (feed == NULL) {
        printf("Error while opening file.\n");
        return 0;
    }

    /* Write bytes to feed. */
    fwrite(buffer, size, 1, feed);
}

/**
 * Write the output of status update message.
 */
void writeStatOutput(uint8_t *buffer, size_t size) {
    printf("Received status update: ");
    printf((const char*)buffer);
}