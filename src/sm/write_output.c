#include "./../../include/sm/write_output.h"

/**
 * Write the output of command message.
 */
void writeCommOutput(uint8_t *buffer, size_t size) {
	printf("Received command: ");
	printf("%s\n", buffer);

	if (equalByteArrays(buffer, COMMAND_STAT, COMMAND_STAT_LN))
		STAT_ACTIVE = 1;
	if (equalByteArrays(buffer, COMMAND_START_FEED, COMMAND_START_FEED_LN))
		FEED_ACTIVE = 1;
	if (equalByteArrays(buffer, COMMAND_STOP_FEED, COMMAND_STOP_FEED_LN))
		FEED_ACTIVE = 0;
}


#if LIVE_FEED_PORT
/**
 * Write the output of video feed message.
 */
void writeFeedOutput(uint8_t *buffer, size_t size) {
	if (size != 0)
		send_feed(buffer, size);
}
#else
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
#endif

/**
 * Write the output of status update message.
 */
void writeStatOutput(uint8_t *buffer, size_t size) {
	printf("Received status update: ");
	printf("%s\n", buffer);
}