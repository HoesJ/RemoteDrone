#include "./../../include/sm/write_output.h"

/**
 * Write the output of command message.
 */
void writeCommOutput(uint8_t *buffer, size_t size) {
    printf("Received command: ");
    printf("%s", buffer);
}

/**
 * Write the output of video feed message.
 */
void writeFeedOutput(uint8_t *buffer, size_t size) {
    static uint8_t rem = 0;
    FILE *feed;

    /* Remove file the first time. */
    if (!rem) {
        rem = 1;
        remove(FEED_OUTPUT);
    }

    /* Open feed. */
    feed = fopen(FEED_OUTPUT, "ab");

    if (feed == NULL) {
        printf("Error while opening file.\n");
        return;
    }

    /* Write bytes to feed. */
    fwrite(buffer, size, 1, feed);

	/* Close file. */
	fclose(feed);
}

/**
 * Write the output of status update message.
 */
void writeStatOutput(uint8_t *buffer, size_t size) {
    printf("Received status update: ");
    printf("%s", buffer);
}