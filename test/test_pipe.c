#include "./../include/test.h"

#define MSG_LEN 14
#define BUFFER_LEN 100

void test_pipe(word *nb) {
    struct IO_ctx sender, receiver;
    char message[MSG_LEN + 1] = "Test message\0\1";
    char result[BUFFER_LEN + 1];
    ssize_t length;
	int pipeToDrone[2];

    pipe(pipeToDrone);
	
    /* Create sender and receiver pipe. */
    init_IO_ctx(&sender, pipeToDrone[1], pipeToDrone[0]);
    init_IO_ctx(&receiver, pipeToDrone[1], pipeToDrone[0]);

    /* Send test message */
    send(&sender, message, MSG_LEN, 1);

    /* Receive test message. */
    do {
        length = receive(&receiver, result, BUFFER_LEN, 1);
    } while (!receiver.endOfMessage);

    assert(compareByteArrays(message, result, MSG_LEN));
    printf("Test %u - Pipe send/receive message passed.\n", (*nb)++);



    /* Send test message */
    send(&sender, message, MSG_LEN, 0);
    send(&sender, message, MSG_LEN, 1);

    /* Receive test message. */
    do {
        length = receive(&receiver, result, BUFFER_LEN, 1);
    } while (!receiver.endOfMessage);

    assert(compareByteArrays(message, result, MSG_LEN));
    assert(compareByteArrays(message, result + MSG_LEN, MSG_LEN));
    printf("Test %u - Pipe send/receive double message passed.\n", (*nb)++);



    /* Send test message */
    send(&sender, message, MSG_LEN, 1);
    send(&sender, message, MSG_LEN, 1);

    /* Receive test message. */
    do {
        length = receive(&receiver, result, BUFFER_LEN, 1);
    } while (!receiver.endOfMessage);

    assert(compareByteArrays(message, result, MSG_LEN));

    /* Receive test message. */
    do {
        length = receive(&receiver, result, BUFFER_LEN, 1);
    } while (!receiver.endOfMessage);

    assert(compareByteArrays(message, result, MSG_LEN));
    printf("Test %u - Pipe send/receive two messages passed.\n", (*nb)++);
}