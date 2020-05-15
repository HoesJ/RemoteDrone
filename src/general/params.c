#include "./../../include/general/params.h"

const word ALL_ONE_MASK  = 0xFFFFFFFF;
const word LEFT_ONE_MASK = 0x80000000;

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

const word rp_2[SIZE] = { 0x00000003,
                          0x00000000,
                          0xFFFFFFFF,
                          0xFFFFFFFB,
                          0xFFFFFFFE,
                          0xFFFFFFFF,
                          0xFFFFFFFD,
                          0x00000004 };

const word n[SIZE] = { 0xFC632551,
                       0xF3B9CAC2,
                       0xA7179E84,
                       0xBCE6FAAD,
                       0xFFFFFFFF,
                       0xFFFFFFFF,
                       0x00000000,
                       0xFFFFFFFF };

const word n_prime[SIZE] = { 0xEE00BC4F,
                             0xCCD1C8AA,
                             0x7D74D2E4,
                             0x48C94408,
                             0xC588C6F6,
                             0x50FE77EC,
                             0xA9D6281C,
                             0x60D06633 };

const word rn_2[SIZE] = { 0xBE79EEA2,
                          0x83244C95,
                          0x49BD6FA6,
                          0x4699799C,
                          0x2B6BEC59,
                          0x2845B239,
                          0xF3D95620,
                          0x66E12D94 };

const word zero[SIZE] = { 0x00000000,
                          0x00000000,
                          0x00000000,
                          0x00000000,
                          0x00000000,
                          0x00000000,
                          0x00000000,
                          0x00000000 };

const word one[SIZE] = { 0x00000001,
                         0x00000000,
                         0x00000000,
                         0x00000000,
                         0x00000000,
                         0x00000000,
                         0x00000000,
                         0x00000000 };

const word one_mont[SIZE] = { 0x00000001,
                              0x00000000,
                              0x00000000,
                              0xFFFFFFFF,
                              0xFFFFFFFF,
                              0xFFFFFFFF,
                              0xFFFFFFFE,
                              0x00000000 };

const word a[SIZE] = { 0xFFFFFFFC,
                       0xFFFFFFFF,
                       0xFFFFFFFF,
                       0x00000000,
                       0x00000000,
                       0x00000000,
                       0x00000001,
                       0xFFFFFFFF };

const word a_mont[SIZE] = { 0xFFFFFFFC,
                            0xFFFFFFFF,
                            0xFFFFFFFF,
                            0x00000003,
                            0x00000000,
                            0x00000000,
                            0x00000004,
                            0xFFFFFFFC };

const word b[SIZE] = { 0x27d2604b,
                       0x3bce3c3e,
                       0xcc53b0f6,
                       0x651d06b0,
                       0x769886bc,
                       0xb3ebbd55,
                       0xaa3a93e7,
                       0x5ac635d8 };

const word b_mont[SIZE] = { 0x29C4BDDF,
                            0xD89CDF62,
                            0x78843090,
                            0xACF005CD,
                            0xF7212ED6,
                            0xE5A220AB,
                            0x04874834,
                            0xDC30061D };

const word c[SIZE] = { 0x0104fa0d,
                       0xaf317768,
                       0xc5114abc,
                       0xce8d84a9,
                       0x75d4f7e0,
                       0x03cb055c,
                       0x2985be94,
                       0x7efba166 };

const word g_x[SIZE] = { 0xd898c296,
                         0xf4a13945,
                         0x2deb33a0,
                         0x77037d81,
                         0x63a440f2,
                         0xf8bce6e5,
                         0xe12c4247,
                         0x6b17d1f2 };

const word g_x_mont[SIZE] = { 0x18A9143C,
                              0x79E730D4,
                              0x5FEDB601,
                              0x75BA95FC,
                              0x77622510,
                              0x79FB732B,
                              0xA53755C6,
                              0x18905F76 };

const word g_y[SIZE] = { 0x37bf51f5,
                         0xcbb64068,
                         0x6b315ece,
                         0x2bce3357,
                         0x7c0f9e16,
                         0x8ee7eb4a,
                         0xfe1a7f9b,
                         0x4fe342e2 };

const word g_y_mont[SIZE] = { 0xCE95560A,
                              0xDDF25357,
                              0xBA19E45C,
                              0x8B4AB8E4,
                              0xDD21F325,
                              0xD2E88688,
                              0x25885D85,
                              0x8571FF18 };

word privDrone[SIZE];
word pkxDrone[SIZE];
word pkyDrone[SIZE];
word privBS[SIZE];
word pkxBS[SIZE];
word pkyBS[SIZE];