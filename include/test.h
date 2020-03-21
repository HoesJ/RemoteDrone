#include <assert.h>
#include "params.h"
#include "aes.h"
#include "aegis.h"
#include "mod_arithmetic.h"
#include "ec_arithmetic.h"
#include "compare_arrays.h"
#include "sha3.h"

#ifndef _TEST_H_
#define _TEST_H_

int test_aes(word* nb);
int test_aegis(word* nb);
int test_arithmetic(word* nb);
int test_ec_arithmetic(word* nb);

#endif