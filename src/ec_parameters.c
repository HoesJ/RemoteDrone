#include "./../include/ec_parameters.h"

const word p[SIZE] = { 0xFFFFFFFF,
                       0xFFFFFFFF,
                       0xFFFFFFFF,
                       0x00000000,
                       0x00000000,
                       0x00000000,
                       0x00000001,
                       0xFFFFFFFF };

const word p_prime[SIZE] = { 0x00000001,
                             0x00000000,
                             0x00000000,
                             0x00000001,
                             0x00000000,
                             0x00000000,
                             0x00000002,
                             0xFFFFFFFF };

const word r_2[SIZE] = { 0x00000003,
                         0x00000000,
                         0xFFFFFFFF,
                         0xFFFFFFFB,
                         0xFFFFFFFE,
                         0xFFFFFFFF,
                         0xFFFFFFFD,
                         0x00000004 };

const word one[SIZE] = { 0x00000001,
                         0x00000000,
                         0x00000000,
                         0x00000000,
                         0x00000000,
                         0x00000000,
                         0x00000000,
                         0x00000000 };

const word a[SIZE] = { 0xFFFFFFFC,
                       0xFFFFFFFF,
                       0xFFFFFFFF,
                       0x00000000,
                       0x00000000,
                       0x00000000,
                       0x00000001,
                       0xFFFFFFFF };

const word b[SIZE] = { 0x27d2604b,
                       0x3bce3c3e,
                       0xcc53b0f6,
                       0x651d06b0,
                       0x769886bc,
                       0xb3ebbd55,
                       0xaa3a93e7,
                       0x5ac635d8 };

const word c[SIZE] = { 0x0104fa0d,
                       0xaf317768,
                       0xc5114abc,
                       0xce8d84a9,
                       0x75d4f7e0,
                       0x03cb055c,
                       0x2985be94,
                       0x7efba166 };