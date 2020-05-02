#include "./../include/test/test.h"

#define MSG_LEN 14
#define BUFFER_LEN 100

void test_pipe(word *nb) {
    struct IO_ctx sender, receiver;
    char message[MSG_LEN + 1] = "Test message\0\1";
    char result[BUFFER_LEN + 1];
	int commPipe[2];
    uint8_t randomBytes[4055], receivedBytes[4055];
    uint16_t index;

    pipe(commPipe);
	
    /* Create sender and receiver pipe. */
    init_IO_ctx(&sender, commPipe[1], commPipe[0]);
    init_IO_ctx(&receiver, commPipe[1], commPipe[0]);

    /* Send test message. */
    transmit(&sender, message, MSG_LEN, 1);

    /* Receive test message. */
    do {
        receive(&receiver, result, BUFFER_LEN, 1);
    } while (!receiver.endOfMessage);

    assert(equalByteArrays(message, result, MSG_LEN));
    printf("Test %u - Pipe send/receive message passed.\n", (*nb)++);



    /* Send test message. */
    transmit(&sender, message, MSG_LEN, 0);
    transmit(&sender, message, MSG_LEN, 1);

    /* Receive test message. */
    do {
        receive(&receiver, result, BUFFER_LEN, 1);
    } while (!receiver.endOfMessage);

    assert(equalByteArrays(message, result, MSG_LEN));
    assert(equalByteArrays(message, result + MSG_LEN, MSG_LEN));
    printf("Test %u - Pipe send/receive double message passed.\n", (*nb)++);



    /* Send test message. */
    transmit(&sender, message, MSG_LEN, 1);
    transmit(&sender, message, MSG_LEN, 1);

    /* Receive test message. */
    do {
        receive(&receiver, result, BUFFER_LEN, 1);
    } while (!receiver.endOfMessage);

    assert(equalByteArrays(message, result, MSG_LEN));

    /* Receive test message. */
    do {
        receive(&receiver, result, BUFFER_LEN, 1);
    } while (!receiver.endOfMessage);

    assert(equalByteArrays(message, result, MSG_LEN));
    printf("Test %u - Pipe send/receive two messages passed.\n", (*nb)++);

    /* Keep sending random stuff. */
    for (index = 0; index < 100; index++) {
        getRandomBytes(2054, randomBytes);
        transmit(&sender, randomBytes, 2054, 1);
        
        /* Receive message. */
        do {
            receive(&receiver, receivedBytes, 2055, 1);
        } while (!receiver.endOfMessage);

        assert(equalByteArrays(randomBytes, receivedBytes, 2054));
    }

    printf("Test %u - Pipe send/receive hundred random messages passed.\n", (*nb)++);
}