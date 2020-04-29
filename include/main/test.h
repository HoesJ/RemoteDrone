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
#include "./../sm/pipe_io.h"
#include "./../sm/udp.h"

#ifndef TEST_H_
#define TEST_H_

void test_aes(word *nb);
void test_aegis(word *nb);
void test_mod_arithmetic(word *nb);
void test_ec_arithmetic(word *nb);
void test_ecdsa(word *nb);
void test_pipe(word *nb);
void test_endianness(word *nb);
void test_udp(word *nb);

#endif