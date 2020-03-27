#include <assert.h>
#include "params.h"
#include "aes.h"
#include "aegis.h"
#include "mod_arithmetic.h"
#include "ec_arithmetic.h"
#include "compare_arrays.h"
#include "sha3.h"
#include "ecdsa.h"
#include "session_info.h"
#include "pipe_io.h"

#ifndef TEST_H_
#define TEST_H_

void test_aes(word *nb);
void test_aegis(word *nb);
void test_mod_arithmetic(word *nb);
void test_ec_arithmetic(word *nb);
void test_ecdsa(word *nb);
void test_pipe(word *nb);

#endif