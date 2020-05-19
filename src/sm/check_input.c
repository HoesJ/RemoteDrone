#include "./../../include/sm/check_input.h"

/* Initialize global flags */
uint8_t FEED_ACTIVE = 0;
uint8_t STAT_ACTIVE = 0;
uint64_t LAST_CHECK = 0;
uint64_t MICRO_INTERVAL = 0;

#ifndef LIVE_FEED
/* Can be reset when session is cleared */
uint8_t feedOpen = 0, feedClosed = 0;
static FILE *feed;

size_t openFeed() {
	feed = fopen(FEED_INPUT, "rb");
	feedOpen = 1;

	if (feed == NULL) {
		feedOpen = 0;
		printf("Error while opening the video feed.\n");
		return 0;
	}

	printf("Drone\t- Starting up video feed\n");
	return 1;
}

void closeFeed() {
	if (feedOpen) {
		feedOpen = 0;
		feedClosed = 1;
		fclose(feed);
		printf("Drone\t- Closing video feed\n");
	}
}
#endif

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
    if (size < COMMAND_LN) {
        return 0;
    }
    
	fromKeyboard = readChar();
	switch (fromKeyboard) {
	case 'c':
		printf("BS\t- Requesting status update.\n");
		memcpy(buffer, commandStat, COMMAND_STAT_LN);
        memset(buffer + COMMAND_STAT_LN, 0, COMMAND_LN - COMMAND_STAT_LN);
		return COMMAND_LN;
	case 'f':
		printf("BS\t- Starting video feed.\n");
		memcpy(buffer, commandStartFeed, COMMAND_START_FEED_LN);
        memset(buffer + COMMAND_START_FEED_LN, 0, COMMAND_LN - COMMAND_START_FEED_LN);
		return COMMAND_LN;
	case 'g':
		printf("BS\t- Stopping video feed.\n");
		memcpy(buffer, commandStopFeed, COMMAND_STOP_FEED_LN);
        memset(buffer + COMMAND_STOP_FEED_LN, 0, COMMAND_LN - COMMAND_STOP_FEED_LN);
		return COMMAND_LN;
	default:
		writeChar(fromKeyboard);
		return 0;
	}
}

#ifdef LIVE_FEED
uint8_t feedBuffer[FEED_BUFFER_SIZE];
int32_t	readOffset;
int32_t	writeOffset;
uint8_t FEED_THREAD_STARTED;

/**
 * Constantly monitors the input feed stream and stores the data in a buffer
 */
void *monitorFeedInput(void* uselessPtr) {
	int spaceLeft;

	while (1) {
		int32_t received;

		if (!FEED_ACTIVE)
			continue;

		spaceLeft = FEED_BUFFER_SIZE - (writeOffset - readOffset >= 0 ? writeOffset - readOffset : writeOffset + FEED_BUFFER_SIZE - readOffset);
		if (spaceLeft < MP4_UDP_SIZE) {
			writeOffset = 5 * MP4_UDP_SIZE + readOffset; /* Means we are overrunning our buffer --> throw old stuff away to decrease latency */
			printf("FEED\t- Buffer overflow\n");
		}

		if (writeOffset + MP4_UDP_SIZE >= FEED_BUFFER_SIZE)
			writeOffset = 0;
	
		received = receive_feed(feedBuffer + writeOffset);
		if (received != -1)
			writeOffset += received;
	}
}

/**
 * Check whether there is feed input available. Return the number of
 * bytes written.
 */
size_t checkFeedInput(uint8_t* buffer, size_t size) {
	uint32_t available;
	uint32_t toRead;
	static pthread_t thread;

	if (!FEED_ACTIVE)
		return 0;

	if (!FEED_THREAD_STARTED) {
		pthread_create(&thread, NULL, monitorFeedInput, NULL);
		FEED_THREAD_STARTED = 1;
	}

	/* Check how much there is available to read */
	available = writeOffset >= readOffset ? writeOffset - readOffset : writeOffset + FEED_BUFFER_SIZE - readOffset;
	if (available <= 0)
		return 0;

	if (readOffset + MP4_UDP_SIZE >= FEED_BUFFER_SIZE)
		readOffset = 0;
	
	toRead = available <= size ? available : size;
	memcpy(buffer, feedBuffer + readOffset, toRead);
	readOffset += toRead;

	return toRead;
}

#else

/**
 * Check whether there is feed input available. Return the number of
 * bytes written.
 */
size_t checkFeedInput(uint8_t *buffer, size_t size) {
	size_t count;

	/* If feed was closed already, no bytes are read. */
	if (feedClosed || !FEED_ACTIVE)
		return 0;

	/* Open feed if necessary. */
	if (!feedOpen && !openFeed())
		return 0;

	/* Read bytes from feed. */
	count = fread(buffer, 1, size, feed);

	/* Close file if no bytes left. */
	if (count != size)
		closeFeed();

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