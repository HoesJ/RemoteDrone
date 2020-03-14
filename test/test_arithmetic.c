#include <assert.h>
#include "./../include/arithmetic.h"

word compareArrays(word *ar1, word *ar2) {
    word i;
    
    for (i = 0; i < SIZE; i++)
        if (ar1[i] != ar2[i])
            return 0;
    
    return 1;
}

void test_mod_add(word *nbTest) {
    word result[SIZE];

    word N[SIZE] = {
        0xe87c0891, 0xedefbacf, 0xbe75c544, 0xb29e5e19, 0x860c5ab6, 0xf1e491df,
        0xc8d459fe, 0xc811c225, 0xc4870bec, 0x60af8542, 0xec8422a1, 0xa5d7ac12,
        0x5e3ad006, 0xc99e51d9, 0xfb0db87f, 0xbf7d0d0c, 0x51777580, 0x77f88d79,
        0x7437df95, 0x96787032, 0x38cfc8ca, 0xba87a2b5, 0x1de9b64c, 0x56905a55,
        0xdf056077, 0x02edd0b8, 0x3a8af36b, 0x631da6cb, 0xde2f29a8, 0x3982f519,
        0xb51e533c, 0xddbe8494};
    word a[SIZE] = {
        0x40ea3679, 0x13b7e95f, 0x961bf864, 0xd09d0a0a, 0x72ed6a7b, 0x2ac4de5b,
        0x8b9ebd95, 0xcf2cdf20, 0x9b729071, 0x39e2ff0a, 0x8f04fa45, 0x9b16fe5a,
        0x79366180, 0xff9d8257, 0x1060d96f, 0x3de121ba, 0xb30033c8, 0xaf5943cd,
        0x65703523, 0xb2630421, 0x6d0e6319, 0xc99c9aa4, 0x45e02225, 0xac1523c7,
        0x50471970, 0xd4cddec8, 0x7489978a, 0xa698959d, 0x4230dc65, 0xba17abc1,
        0x15b5dc9b, 0xaca3a8e7};
    word b[SIZE] = {
        0x283c7403, 0x4357f769, 0xab14c306, 0x4ad81b52, 0xb1fc5913, 0x35c9dfe9,
        0x7c1d80e8, 0x54e6e780, 0xe89ae1e9, 0x8501757c, 0xab3da57b, 0xecd1aa87,
        0xcebe9311, 0x3a67a0e9, 0xce9c9a13, 0x1df20725, 0x6bf1e858, 0xda4701c3,
        0xcd8b2db7, 0xd542dbd0, 0xb7f8f516, 0xdc353e6f, 0x8f9bbcea, 0x5f3dab0b,
        0x3e9ebf0f, 0xb1965516, 0x6ecff295, 0x40591cbb, 0x089cdff2, 0x95401932,
        0xc6d31d88, 0xa4bc1304};
    word c[SIZE] = {
        0xc6c3c93a, 0x7051794e, 0x000ca427, 0xeafa3218, 0x45693a60, 0xeceb5ca2,
        0x91af509c, 0x5127beff, 0x36746c50, 0x13117383, 0x16fcd75e, 0x79023bd8,
        0xeea9e87e, 0x8b9f6349, 0xc89ad6fe, 0x2004481b, 0xed4a0343, 0x6512fdff,
        0x29ade3a6, 0xa1a09e57, 0x061b5640, 0xfccfe580, 0x85d3881a, 0x7cdebf3c,
        0xf8895370, 0xb36d5fe7, 0xb250463e, 0x6de85c82, 0x0bf498b0, 0xaf834d0a,
        0x2e3b67b7, 0x1ccc03f7};
    word expected1[SIZE] = {
        0x80aaa1eb, 0x692025f8, 0x82baf625, 0x68d6c743, 0x9edd68d8, 0x6eaa2c65,
        0x3ee7e47e, 0x5c02047b, 0xbf86666e, 0x5e34ef44, 0x4dbe7d1f, 0xe210fccf,
        0xe9ba248b, 0x7066d167, 0xe3efbb03, 0x9c561bd2, 0xcd7aa69f, 0x11a7b817,
        0xbec38346, 0xf12d6fbf, 0xec378f65, 0xeb4a365e, 0xb79228c3, 0xb4c2747d,
        0xafe07808, 0x83766325, 0xa8ce96b5, 0x83d40b8d, 0x6c9e92af, 0x15d4cfd9,
        0x276aa6e8, 0x73a13757};
    word expected2[SIZE] = {
        0x07adffb3, 0x840962ae, 0x96289c8b, 0xbb973c22, 0xb856a4dc, 0x17b03afd,
        0x1d4e0e32, 0x20549e20, 0xd1e6fcc2, 0x4cf4728d, 0xa601d1a3, 0x14193a32,
        0x67e049ff, 0x8b3ce5a1, 0xd8fbb06e, 0x5de569d5, 0xa04a370b, 0x146c41cd,
        0x8f1e18ca, 0x5403a278, 0x7329b95a, 0xc66c8024, 0xcbb3aa40, 0x28f3e303,
        0x48d06ce1, 0x883b3eb0, 0x26d9ddc9, 0x1480f220, 0x4e257516, 0x699af8cb,
        0x43f14453, 0xc96facde};

    mod_add(a, b, N, result);
    assert(compareArrays(expected1, result));
    printf("Test %u - Modular addition passed.\n", (*nbTest)++);

    mod_add(a, c, N, result);
    assert(compareArrays(expected2, result));
    printf("Test %u - Modular addition passed.\n", (*nbTest)++);
}

void test_mod_sub(word *nbTest) {
    word result[SIZE];

    word N[SIZE] = {
        0xe87c0891, 0xedefbacf, 0xbe75c544, 0xb29e5e19, 0x860c5ab6, 0xf1e491df,
        0xc8d459fe, 0xc811c225, 0xc4870bec, 0x60af8542, 0xec8422a1, 0xa5d7ac12,
        0x5e3ad006, 0xc99e51d9, 0xfb0db87f, 0xbf7d0d0c, 0x51777580, 0x77f88d79,
        0x7437df95, 0x96787032, 0x38cfc8ca, 0xba87a2b5, 0x1de9b64c, 0x56905a55,
        0xdf056077, 0x02edd0b8, 0x3a8af36b, 0x631da6cb, 0xde2f29a8, 0x3982f519,
        0xb51e533c, 0xddbe8494};
    word a[SIZE] = {
        0x40ea3679, 0x13b7e95f, 0x961bf864, 0xd09d0a0a, 0x72ed6a7b, 0x2ac4de5b,
        0x8b9ebd95, 0xcf2cdf20, 0x9b729071, 0x39e2ff0a, 0x8f04fa45, 0x9b16fe5a,
        0x79366180, 0xff9d8257, 0x1060d96f, 0x3de121ba, 0xb30033c8, 0xaf5943cd,
        0x65703523, 0xb2630421, 0x6d0e6319, 0xc99c9aa4, 0x45e02225, 0xac1523c7,
        0x50471970, 0xd4cddec8, 0x7489978a, 0xa698959d, 0x4230dc65, 0xba17abc1,
        0x15b5dc9b, 0xaca3a8e7};
    word b[SIZE] = {
        0x283c7403, 0x4357f769, 0xab14c306, 0x4ad81b52, 0xb1fc5913, 0x35c9dfe9,
        0x7c1d80e8, 0x54e6e780, 0xe89ae1e9, 0x8501757c, 0xab3da57b, 0xecd1aa87,
        0xcebe9311, 0x3a67a0e9, 0xce9c9a13, 0x1df20725, 0x6bf1e858, 0xda4701c3,
        0xcd8b2db7, 0xd542dbd0, 0xb7f8f516, 0xdc353e6f, 0x8f9bbcea, 0x5f3dab0b,
        0x3e9ebf0f, 0xb1965516, 0x6ecff295, 0x40591cbb, 0x089cdff2, 0x95401932,
        0xc6d31d88, 0xa4bc1304};
    word c[SIZE] = {
        0xc6c3c93a, 0x7051794e, 0x000ca427, 0xeafa3218, 0x45693a60, 0xeceb5ca2,
        0x91af509c, 0x5127beff, 0x36746c50, 0x13117383, 0x16fcd75e, 0x79023bd8,
        0xeea9e87e, 0x8b9f6349, 0xc89ad6fe, 0x2004481b, 0xed4a0343, 0x6512fdff,
        0x29ade3a6, 0xa1a09e57, 0x061b5640, 0xfccfe580, 0x85d3881a, 0x7cdebf3c,
        0xf8895370, 0xb36d5fe7, 0xb250463e, 0x6de85c82, 0x0bf498b0, 0xaf834d0a,
        0x2e3b67b7, 0x1ccc03f7};
    word expected1[SIZE] = {
        0x18adc276, 0xd05ff1f6, 0xeb07355d, 0x85c4eeb7, 0xc0f11168, 0xf4fafe71,
        0x0f813cac, 0x7a45f7a0, 0xb2d7ae88, 0xb4e1898d, 0xe3c754c9, 0xae4553d2,
        0xaa77ce6e, 0xc535e16d, 0x41c43f5c, 0x1fef1a94, 0x470e4b70, 0xd512420a,
        0x97e5076b, 0xdd202850, 0xb5156e02, 0xed675c34, 0xb644653a, 0x4cd778bb,
        0x11a85a61, 0x233789b2, 0x05b9a4f5, 0x663f78e2, 0x3993fc73, 0x24d7928f,
        0x4ee2bf13, 0x07e795e2};
    word expected2[SIZE] = {
        0x87035dc8, 0x1ae93cb5, 0x136da666, 0x52c074df, 0x19793c04, 0xa9060e98,
        0xde6629b3, 0xc45299a4, 0x12609653, 0xeebf8349, 0x58435483, 0x32083d63,
        0x7e262573, 0x1ad61439, 0xf50bf56b, 0xc18f4e02, 0xd2cf906b, 0x02c489b5,
        0xd05a9584, 0x62d632b8, 0x86f229f4, 0xdb2249c5, 0x1421817c, 0x74316e86,
        0x98eff4d8, 0x04c4db8a, 0x7e0b4714, 0x90ace692, 0xe186e266, 0x53c628f1,
        0x1c869d6b, 0x55ce7587};

    mod_sub(a, b, N, result);
    assert(compareArrays(expected1, result));
    printf("Test %u - Modular subtraction passed.\n", (*nbTest)++);

    mod_sub(c, b, N, result);
    assert(compareArrays(expected2, result));
    printf("Test %u - Modular subtraction passed.\n", (*nbTest)++);
}

void test_montgomery(word *nbTest) {
    word res[SIZE + 1];

    word a[SIZE] = {
        0xa2e47eba, 0xdb5e3e7d, 0x5fd9a7cb, 0x9e7a37c3, 0x4dc37416, 0x834fe315,
        0x0080dbac, 0x47280a4d, 0x722dc3dd, 0x7f0ca6f4, 0x7f0da3db, 0xfc20cbb9,
        0xb749699e, 0xc5abc925, 0x8bc67669, 0x23a30d3f, 0xffe44e1c, 0xdb22d6ee,
        0x5dc3d3b3, 0xfe7e980d, 0x894075d2, 0xdeabe7c4, 0x58042c3b, 0x1cc18107,
        0xf0abb2e1, 0x7bd31dc0, 0x72355db9, 0xf55a0178, 0x99f5d831, 0x12ccb47a,
        0x44eef089, 0x81bf5831};
    word b[SIZE] = {
        0xe47d092a, 0xd7615ed9, 0xa8aff661, 0xc3d7bb8c, 0x2d22b31c, 0x5050b808,
        0xb0bc44bb, 0x93e46f4f, 0xac0332e5, 0x81ad7f85, 0x23de9197, 0x4b35f0a2,
        0x6303744a, 0x615740bf, 0x54ebbaef, 0xf36273c2, 0xaf0d1080, 0x4740c9d6,
        0x458e6699, 0xaaa1040b, 0x614df162, 0xaf7fe30d, 0xac73e3ed, 0x49204145,
        0x1653523c, 0x5ee4f3b1, 0x2b3f4339, 0x5b9a2426, 0x62e3ffb3, 0xf4c32df1,
        0x94f2db82, 0x80deb209};
    word n[SIZE] = {
        0xb2e52d11, 0x8f850a5f, 0x08edfb0c, 0x4f02a1ac, 0xdd070636, 0x6c59cfc8,
        0x16a40db7, 0x85bba009, 0x598eba07, 0x5d0fa3cc, 0x32bc6fb5, 0x4aa27cd0,
        0x0928454e, 0x027d3081, 0xd1caec35, 0xa98e4223, 0x533983de, 0x36370bb4,
        0xe8b0b4c6, 0xfe3655ab, 0x27a4a51e, 0xb403c449, 0xa679cf3b, 0xc1fc4391,
        0x884e5080, 0xa65d4d30, 0xcd90ed67, 0x55fcb457, 0x989d6f4c, 0x0303d70e,
        0x9a09328e, 0x82260dd0};
    word n_prime[SIZE] = {
        0xfb5c9c0f, 0xd3cbec85, 0x3a126c50, 0x0f85259a, 0xb93b900a, 0x89feafbe,
        0x30847469, 0x0a87f885, 0x9d5bfa86, 0xec1adae0, 0x448a3dac, 0xa7194e08,
        0x61c37198, 0xd5eff8a8, 0xf96efef1, 0x473d83aa, 0xe7c048dd, 0xb1893377,
        0xc843e910, 0xcd3b86d2, 0x2119b9dd, 0x6caed27a, 0x7ebcc873, 0x2019cd09,
        0x240e3495, 0x040754fb, 0xdfef2830, 0x32a287d7, 0x8e79f7e4, 0x3daeb7e0,
        0xbeaeac00, 0x376725cd};
    word sol[SIZE] = {
        0x919af36d, 0x201c5a15, 0x763debb4, 0x54b92abd, 0xc3d09af8, 0x0269454a,
        0x35a42e09, 0x1149a6e6, 0x381cfa5b, 0x61b91a68, 0x7180faf5, 0x8c7536c6,
        0x0368f56d, 0x0ce9c2ab, 0xd6180f19, 0x0cf8b5ac, 0x62ca69e8, 0x8789e2ec,
        0x080ca2ea, 0x4dea79c7, 0xf3b4499d, 0xbae3cd24, 0x148e24af, 0x83e7cc56,
        0x22548763, 0x1e0f13bf, 0xc6b26d8e, 0xfc9c092d, 0x8fa70ce2, 0xb5098439,
        0x9f7d1f55, 0x50a9e0c6};

    montMul(a, b, n, n_prime, res);
    assert(compareArrays(sol, res));
    printf("Test %u - Montgomery multiplication passed.\n", (*nbTest)++);
}

void test_mod_inv(word *nbTest) {
    word res[SIZE + 1];

    /* TV1 */
    word x1[32] = { 0xbce2b7ba, 0x8e575dfc, 0xbe702673, 0x6df699f5,
                    0x9a24b9d6, 0x08344b0c, 0xf612cf34, 0x8495d104,
                    0x9726ca57, 0x5078eb3b, 0x33bc64c1, 0x4da38808,
                    0x5881d1cb, 0x0dd1e088, 0xcc86efda, 0x3e3f8239,
                    0xb91b3948, 0x8dc14e42, 0x94e663ee, 0x07e78004,
                    0x145a546a, 0x8e39ce00, 0x990adeef, 0x17552038,
                    0x78c7694a, 0x4ad8481e, 0xa2e62ee2, 0x44bbd13e,
                    0x1e5068b7, 0xc90e9b73, 0x4d2487b4, 0xba88efdd};

    word p1[32] = { 0x68edaa09, 0xe5e39130, 0x91c40263, 0x7d7f834c,
                    0x8b28a0fe, 0x742d3a6d, 0xb1f00b7d, 0x5bb18472,
                    0xacc8f8c9, 0xa25612f7, 0xa7fb5bce, 0x08866384,
                    0x16df05e2, 0xee83a388, 0x18a8a3e9, 0x806794ca,
                    0xc31919af, 0x2de138b4, 0x89baa3db, 0xb6ce9208,
                    0x8c886d89, 0x05f498c4, 0x4417e729, 0x79c645d1,
                    0x6e9ba684, 0xc29ba104, 0xd369de42, 0xc010a8b2,
                    0x363bf07e, 0x48cfb405,0x8076ebee, 0xfff5e822};

    word sol1[32] = { 0xb91ccb06, 0x06c2a9de, 0x8d34abdf, 0x10c3af4a,
                      0x93ed0c50, 0x3c076ed1, 0x3df49c5a, 0xdfb21b33,
                      0x611cb61e, 0x40df795d, 0xe8a1f1d3, 0xb13e5c67,
                      0xf67bce99, 0x6ccd6b94, 0xf2f82842, 0x518f5e86,
                      0xff4fa5dc, 0x3cee1968, 0xb51c9005, 0x270aa068,
                      0xc920a723, 0xb9fd4df6, 0xa3530ad0, 0xce398390,
                      0xa0f978c0, 0xb1316ddb, 0xe3218928, 0xc79e63be,
                      0x5fa14a08, 0x784f02f5, 0xcbbf3d6e, 0x35ac551d};

    /* TV2 */
    word x2[32] = { 0x3407488d, 0xd1fba4ee, 0x2bd11fcb, 0xb0d26396,
                    0xa5ea985d, 0x80fd2c80, 0x5061a035, 0x6d025cf3,
                    0xbaf553ce, 0x19389f49, 0xf9e58939, 0x67030b0b,
                    0xfbab17b9, 0x6fe2b59a, 0x51e2c2a3, 0x127a938d,
                    0x07e1eadd, 0xe5945750, 0x0e939dc7, 0x0117364d,
                    0xb527585e, 0x5a5f5884, 0x492c115a, 0x332d95e8,
                    0xcd4724ec, 0x36f530bc, 0x5dd1e30a, 0x95cc3f67,
                    0x53992418, 0x2e72b802, 0xbcaa990f, 0xb5184b5c};

    word p2[32] = { 0xda39c9f9, 0x7c9d66e0, 0x68eb6a83, 0xa3391624,
                    0xe3536e1d, 0xdbbe7236, 0x7f02ab5c, 0x3adb9416,
                    0x29a5b8f1, 0xc77cb8b6, 0x75919b2c, 0xdb9633b9,
                    0xee4030c2, 0xd3f928bc, 0xbcc0d1f0, 0x8ec7f99b,
                    0xed2daccc, 0x48a4ace8, 0xecf5a399, 0x78148be3,
                    0x4f4f531e, 0x2186394e, 0x9dff7c5d, 0x8459048d,
                    0xc34d1da1, 0x87d57f53, 0xef1c992f, 0xd7b560f3,
                    0xfaa9b324, 0x65d97465, 0x0106295a, 0xe257fb85};

    word sol2[32] = { 0xf23d5f0e, 0x6c7cc098, 0x4b43f23a, 0xf1eeabf8,
                      0x1e6dbd31, 0x5966db5a, 0x0961ef14, 0x596bfe58,
                      0x04b6edf6, 0xdcaee9bf, 0x3a975978, 0x30200c63,
                      0x0a22fcf1, 0x6c81009d, 0xdf2671ac, 0xc8a55232,
                      0xa29a774e, 0x04c1bb00, 0xcea8c356, 0x39adbd2c,
                      0x9b458fab, 0x82a8a716, 0xc1530da4, 0xd1d615a0,
                      0x1a8cad8d, 0x50cdceef, 0x6ab17aed, 0x62a9d75d,
                      0x3c196451, 0x9e8d13f4, 0x0e6bcb26, 0x82d7f6d5};

    /* TV3 */
    word x3[32] = { 0x4ea6cbdb, 0xcd839e8b, 0xc9215856, 0x2a364c27,
                    0x326fd5ea, 0x36f4c2c5, 0x13193890, 0xd5c57972,
                    0xc111e46b, 0x555443d1, 0x10900ed8, 0x6abc4f6b,
                    0xd1260614, 0x933809e8, 0x47d7efa3, 0x1a645c61,
                    0xe781a8da, 0x3a54b3f4, 0xf9318484, 0xbe6e8fab,
                    0x31164780, 0x3f918043, 0xbd683b1b, 0x144027f8,
                    0x2b259612, 0xe12a9829, 0x68b698c4, 0xf8f947f9,
                    0x92f70343, 0x9dbc5355, 0x19827ca7, 0x88133402};

    word p3[32] = { 0xbf1cb493, 0x191e1ae5, 0xa75e32d6, 0x29323977,
                    0x6f059284, 0x68385dc8, 0xc1b29b2c, 0x79b15835,
                    0x4cafc270, 0x11e9f1ef, 0x42539f5e, 0xc09b8015,
                    0x5fc63d97, 0x003f8f9f, 0x6cdb6dad, 0xd54bd5ea,
                    0x84d94399, 0x67db11ea, 0x2e3cc164, 0xe6fcda95,
                    0x61f4d888, 0x8e9187e4, 0xf0ccf79b, 0xa72fb83e,
                    0x6264b9c6, 0xe2cd7d2d, 0x90de8e4c, 0x9c2f7eb8,
                    0xe70b9340, 0xb157d479, 0x4cea00db, 0xcf808996};

    word sol3[32] = { 0xd7dc9ea4, 0x3725929c, 0x5a3bc1da, 0x948fa8af,
                      0x54fb0418, 0x33e5733b, 0x4cd7b5a5, 0xca45db52,
                      0x90d3290c, 0x78409150, 0x40a79a97, 0x2cdc4f99,
                      0x0aeadc29, 0x00526fcf, 0x7b923507, 0x07dd7413,
                      0x14f089d5, 0xc67810c7, 0x6d2984a4, 0x13169d40,
                      0xd9860391, 0xd30e460f, 0x66019051, 0xc79b70c9,
                      0x202bab09, 0xedb774e1, 0xdbca2786, 0x0c5d6ac7,
                      0x95adc696, 0xd343958c, 0xb46f1339, 0x703b6c65};

    /* TV4 */
    word x4[32] = { 0x93cd4213, 0x45529a1e, 0x2b33682b, 0x1f06443a,
                    0x8585c610, 0xf0c9ef15, 0x8fc0ea1d, 0xbf459822,
                    0xcfbc5250, 0x8479cbbc, 0xee57b69d, 0x834e5421,
                    0x5057545c, 0x5506fd76, 0x12d8a58c, 0xc44dd13a,
                    0x6def190a, 0x93287ba4, 0xc4256d43, 0x232132c7,
                    0x8eaa7852, 0xd84625ba, 0x79a14701, 0x77154ef5,
                    0xd15e3053, 0xaa125cb2, 0x9a5ea679, 0x70943ff8,
                    0x2020ce4e, 0x1dec2563, 0xb4ea3f41, 0xa59eaa5e};

    word p4[32] = { 0x7a100805, 0xe8c48ec3, 0x1920d5fd, 0x08b61dcf,
                    0xd85603e0, 0x6a7acc04, 0xa6017824, 0x598210d9,
                    0xe3e6af48, 0xe9fc817c, 0x996feecb, 0xcd963993,
                    0x83000989, 0x92366ec4, 0x61e93162, 0xea8e071d,
                    0x3ce569eb, 0x00993af6, 0x83a05f12, 0xd2dc7d29,
                    0x1a0dd95c, 0xa386ecb5, 0xba5fae48, 0xd2c770c9,
                    0x023ab935, 0x64625fb4, 0x52cd4443, 0x5f37c88f,
                    0x98f8e5cc, 0x900c55e3, 0x25a08093, 0xb4a003b8};

    word sol4[32] = { 0x0b9fff45, 0x1901ddc3, 0x734beafc, 0xc9dc3c38,
                      0x9c86b17e, 0x878618b7, 0x4259260b, 0xcded6272,
                      0x677eb236, 0xcba0570c, 0xf1c708f5, 0x4b15ff92,
                      0xb65ace6d, 0x430672a5, 0x67625ca6, 0x1d400dd4,
                      0xf6794940, 0xa3b28b30, 0x69c56f1c, 0x9b231641,
                      0x43c9814a, 0xa39cf836, 0xfc3f1ff9, 0x7badf066,
                      0x6249015d, 0x69bcf4a2, 0xae212cf4, 0xc73205ba,
                      0xe3afb2a3, 0x27b358b2, 0xa0e52073, 0x9091d49d};

    /* TV5 */
    word x5[32] = { 0x2ab988c5, 0x696bec9d, 0x8f85fcef, 0x7e703fb4,
                    0xeb50d8cc, 0x4e6a0a00, 0xf20dc7b7, 0x80cf76af,
                    0xd1c66fd0, 0x2d8f1f41, 0x1c144833, 0xc3ad9050,
                    0x304e7ed1, 0x4f4d89b5, 0x86169e90, 0x3d5e963d,
                    0x91ec6c8e, 0x09703e6e, 0x71a15020, 0x33bd5baa,
                    0x6b7d7c36, 0x386ba7d4, 0xb4749a72, 0x40b1ffe3,
                    0x7bf602f6, 0xa2941c8d, 0xb4b6fb23, 0xfcb3f218,
                    0x3deb3aae, 0xfc57a554, 0x3cdadcd9, 0xb36ef4be};

    word p5[32] = { 0xde4adb2d, 0xa950d9fd, 0xae08479f, 0x99f21474,
                    0x5cf8c72d, 0x2327c92c, 0x63740542, 0x1a4f002a,
                    0x065891d1, 0x83212e12, 0x71ba1189, 0x35123c1a,
                    0x10fe1323, 0xc9305fbc, 0x5ec893f7, 0x0210dc19,
                    0x12aa8c0e, 0xdb8beb50, 0xc7973625, 0x86b48580,
                    0x125c8b4e, 0x1e987d57, 0x36535b9d, 0xce732f52,
                    0x4ecc4179, 0xe9d7f19d, 0x2a7f385e, 0xd04f9fef,
                    0x18324805, 0xcfc1f4da, 0xc04fe172, 0xb62e3bb2};

    word sol5[32] = { 0xf6e41423, 0x6c787252, 0x8525c894, 0x81ac71ad,
                      0x851690c6, 0x82768e50, 0x9cd7a8ca, 0xf03d9fa5,
                      0x04533b45, 0x7a7ba889, 0xa120036e, 0x813ccc25,
                      0x5bd7f9fc, 0x8dccc397, 0x39090976, 0xcb2e4ddf,
                      0x04fd3df3, 0x70a26050, 0x0304806b, 0xb19c1f24,
                      0x271db6ed, 0x67b090e8, 0x93c56da0, 0x293cf82b, 
                      0x178bf772, 0x66feabf3, 0xcac17a74, 0x0f7378e7, 
                      0x836260af, 0xdb6e5f39, 0x37ac8c0c, 0x97da2dd2};

    /* Tests */
    mod_inv(x1, p1, res);
    assert(compareArrays(sol1, res));
    mod_inv(x2, p2, res);
    assert(compareArrays(sol2, res));
    mod_inv(x3, p3, res);
    assert(compareArrays(sol3, res));
    mod_inv(x4, p4, res);
    assert(compareArrays(sol4, res));
    mod_inv(x5, p5, res);
    assert(compareArrays(sol5, res));

    printf("Test %u - Modular inversion passed.\n", (*nbTest)++);
}

int main(int argc, char const *argv[])
{
    word nbTest = 1;

    test_mod_add(&nbTest);
    test_mod_sub(&nbTest);
    test_montgomery(&nbTest);
    test_mod_inv(&nbTest);

    return 0;
}