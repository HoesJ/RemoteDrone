#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef PARAMS_H_
#define PARAMS_H_

typedef int32_t  signed_word;
typedef uint32_t word;
typedef uint64_t double_word;

#define BITS 32
#define SIZE 32

/* Lengths derived from word size */
#define AES_BLOCK_NUMEL		16 / sizeof(word)
#define AEGIS_STATE_NUMEL	80 / sizeof(word)

#define AEGIS_S0_OFFSET		0
#define AEGIS_S1_OFFSET		80 / (5 * sizeof(word))
#define AEGIS_S2_OFFSET		AEGIS_S1_OFFSET * 2
#define AEGIS_S3_OFFSET		AEGIS_S1_OFFSET * 3
#define AEGIS_S4_OFFSET		AEGIS_S1_OFFSET * 4

/* Endianess stuff */
/* This is needed when going from a word array to a 
	byte array. for example: 
		word arr[2] = {0x1234578, 0xaaaaaaaa}
		uint8_t bytes[8] = (uint8_t*) arr;

		now:
			bytes[0] = 0x78 on a Little Endian
			bytes[0] = 0x12 on a Big Endian
	I think the Big Endian makes the most sense
	so I would always convert to that.
*/
#define LITTLE_ENDIAN 1
#define BIG_ENDIAN 0

/* Ignore for now - can be avoided by declaring word arrays as byte arrays

#if (BITS == 16)
	#define SWAP_WORD(w) (((w) >> 8) | ((w) << 8))
#elif (BITS == 32)
	#define SWAP_WORD(w)
#endif

*/

				

#endif