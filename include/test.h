#ifndef _TEST_H_
#define _TEST_H_

#include <assert.h>
#include "params.h"
#include "aes.h"
#include "aegis.h"
#include "arithmetic.h"

int test_aes(word* nb);
int test_aegis(word* nb);
int test_arithmetic(word* nb);

#endif