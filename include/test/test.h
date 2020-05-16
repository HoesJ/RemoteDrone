#include <assert.h>
#include "./../crypto/aes.h"
#include "./../crypto/aegis.h"
#include "./../crypto/mod_arithmetic.h"
#include "./../crypto/ec_arithmetic.h"
#include "./../crypto/sha3.h"
#include "./../crypto/ecdsa.h"
#include "./../crypto/crt_drbg.h"
#include "./../general/params.h"
#include "./../general/compare_arrays.h"
#include "./../general/endian_convert.h"
#include "./../sm/session_info.h"
#include "./../sm/udp_io.h"
#include "./../sm/udp.h"
#include "./../sm/enc_dec.h"
#include "./../general/microtime.h"

#ifndef TEST_H_
#define TEST_H_

void test_aes(uint32_t *nb);
void test_aegis(uint32_t *nb);
void test_mod_arithmetic(uint32_t *nb);
void test_ec_arithmetic(uint32_t *nb);
void test_ecdsa(uint32_t *nb);
void test_pipe(uint32_t *nb);
void test_endianness(uint32_t *nb);
void test_udp(uint32_t *nb);
void test_parameters(uint32_t *nb);
void test_throughput(uint32_t *nb);

#endif