#include "./../include/test/test.h"

#define MAX_P_SIZE (FIELD_HEADER_NB + 10000 + AEGIS_MAC_NB)

void test_encryptionOnly(int payload_size) {
	uint8_t packet[MAX_P_SIZE];
	uint8_t target[FIELD_TARGET_NB] = { 0 };
	uint32_t sequenceNb;
	uint8_t IV[AEGIS_IV_NB];
	uint8_t key[AEGIS_KEY_NB];
	struct AEGIS_ctx ctx;
	int correct, i;
	uint64_t currentTime;
	double encryptionTimePassed, decryptionTimePassed;

	encryptionTimePassed = 0;
	decryptionTimePassed = 0;
	for (i = 0; i < 10000; i++) {
		/* Form random packet, as if it was a video feed packet */
		getRandomBytes(4, &sequenceNb);
		getRandomBytes(AEGIS_IV_NB, IV);
		getRandomBytes(AEGIS_KEY_NB, key);
		encodeMessage(packet, TYPE_FEED_SEND, FIELD_HEADER_NB + payload_size + AEGIS_MAC_NB, target, sequenceNb, IV);

		currentTime = getMicrotime();
		/* Encrypt */
		ctx.iv = IV;
		ctx.key = key;
		aegisEncryptMessage(&ctx, packet, FIELD_HEADER_NB, payload_size);
		encryptionTimePassed += (getMicrotime() - currentTime);
		
		currentTime = getMicrotime();
		/* Decrypt */
		ctx.iv = IV;
		ctx.key = key;
		correct = aegisDecryptMessage(&ctx, packet, FIELD_HEADER_NB, payload_size);
		decryptionTimePassed += (getMicrotime() - currentTime);

		/* printf("Iteration %d: %d\n", i, correct); */
	}

	printf("Encryption for payloads of %d --> %f Mbps\n", payload_size, 10000.0 * payload_size / encryptionTimePassed);
	printf("Decryption for payloads of %d --> %f Mbps\n", payload_size, 10000.0 * payload_size / decryptionTimePassed);
}

void test_throughput(uint32_t *nb) {
	test_encryptionOnly(100);
	test_encryptionOnly(200);
	test_encryptionOnly(500);
	test_encryptionOnly(1000);
	test_encryptionOnly(1500);
	test_encryptionOnly(2500);
	test_encryptionOnly(5000);
	test_encryptionOnly(10000);
}