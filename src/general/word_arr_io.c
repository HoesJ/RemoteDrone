#include "./../../include/general/word_arr_io.h"

/**
 * Write the given word array to a file in little endian representation.
 */
void writeWordArrToFile(word *arr, const char *filePath) {
    FILE *file;
    uint8_t arrLittleEndian[SIZE * sizeof(word)];

    /* Open file to write word array */
    file = fopen(filePath, "wb");

    if (file == NULL) {
		printf("Error while opening file.\n");
		return;
	}

    /* Convert array to little endian */
    toLittleEndian(arr, arrLittleEndian, SIZE);

    /* Write bytes and close file */
	fwrite(arrLittleEndian, SIZE * sizeof(word), 1, file);
	fclose(file);
}

/**
 * Read a little endian represented word array from a file.
 */
void readWordArrFromFile(word *arr, const char *filePath) {
    FILE *file;
    uint8_t arrLittleEndian[SIZE * sizeof(word)];
    uint32_t count;

    /* Open file to read word array */
    file = fopen(filePath, "rb");

    if (file == NULL) {
		printf("Error while opening file.\n");
		return;
	}

    /* Read bytes and close file */
    count = fread(arrLittleEndian, 1, SIZE * sizeof(word), file);
    if (count < SIZE * sizeof(word)) {
        printf("File does not contain valid data.\n");
		return;
    }
	fclose(file);

    /* Convert little endian representation to word array */
    toWordArray(arrLittleEndian, arr, SIZE);
}