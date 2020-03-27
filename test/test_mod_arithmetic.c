#include "./../include/test.h"
#include "./../include/session_info.h"
#include "./../include/pipe_io.h"

void test_mod_add(word *nbTest) {
    word result[SIZE];

	word a1[SIZE] = { 0x15ccf5a2, 0xf97d6573, 0x26df0378, 0x6aee97e9, 0x07d8167c, 0x42f63157, 0x14f7114c, 0xc0f35596 };
	word b1[SIZE] = { 0x5525fa4d, 0xb40bcdce, 0x5269bff0, 0x48e6c667, 0xc879b142, 0x8d0d469c, 0xa25391b3, 0xdac45ebc };
	word n1[SIZE] = { 0x2e4434bb, 0xa15c8d29, 0xf4ce8905, 0xa83d5a3b, 0x21eed46e, 0x6a779bea, 0x5c6f2aff, 0xdc0e1b6e };
	word sol1[SIZE] = { 0x3caebb34, 0x0c2ca618, 0x847a3a64, 0x0b980414, 0xae62f350, 0x658bdc09, 0x5adb7800, 0xbfa998e4 };

	word a2[SIZE] = { 0x88059c7e, 0x183b69c8, 0x440b89c7, 0x7a6291e9, 0x27749427, 0xb3f2fcf0, 0x4fe6c69c, 0x97b94dbd };
	word b2[SIZE] = { 0x11acdb5a, 0xde37cfec, 0xeed73ec7, 0xaf3c7040, 0x98c5a87e, 0xad29856c, 0x97b790fe, 0x8d78c21e };
	word n2[SIZE] = { 0xe4edf69d, 0x9f54ed95, 0xf5859d6a, 0xa0cf3934, 0x538ffb7c, 0xdfd49ed3, 0xd67906ae, 0x9c09ec8e };
	word sol2[SIZE] = { 0xb4c4813b, 0x571e4c1e, 0x3d5d2b24, 0x88cfc8f5, 0x6caa4129, 0x8147e389, 0x112550ec, 0x8928234d };

    mod_add(a1, b1, n1, result);
    assert(compareWordArrays(sol1, result, SIZE));
    printf("Test %u - Modular addition passed.\n", (*nbTest)++);

    mod_add(a2, b2, n2, result);
    assert(compareWordArrays(sol2, result, SIZE));
    printf("Test %u - Modular addition passed.\n", (*nbTest)++);
}

void test_mod_sub(word *nbTest) {
    word result[SIZE];

	word a1[SIZE] = { 0x880275ad, 0xd95e2ae0, 0x4c7ef72c, 0x2a6b3158, 0x907fb897, 0x5e9019dd, 0xc59eca3c, 0x8783c665 };
	word b1[SIZE] = { 0x3511e560, 0xb6c8d24c, 0xd7f88caf, 0x826959a2, 0x2e56f865, 0xfb44db38, 0xe5e89f20, 0x8c7a3114 };
	word n1[SIZE] = { 0x36556101, 0x8effe1ae, 0x3de3ffd8, 0xdfeb8e96, 0x1d9812aa, 0x9445ba3b, 0x9781d2c3, 0x904ca514 };
	word sol1[SIZE] = { 0x8945f14e, 0xb1953a42, 0xb26a6a55, 0x87ed664b, 0x7fc0d2dc, 0xf790f8e0, 0x7737fdde, 0x8b563a65 };

	word a2[SIZE] = { 0xc8eae0ee, 0xbdb7ef87, 0x4a97ee53, 0x4e219503, 0xb7fe87a5, 0xfdbf4ac2, 0xbcdb0a88, 0xbeb5d0ce };
	word b2[SIZE] = { 0xea71a0df, 0xb377ccbc, 0x521464d3, 0xcfb86f9b, 0xd12d3f65, 0x88dece5e, 0xc3797d47, 0xb0427e93 };
	word n2[SIZE] = { 0xa7af3ddd, 0x344a0434, 0x5a894791, 0x961e9204, 0xcf170d65, 0x1cecf0bb, 0x4f717c2f, 0xd917e6e2 };
	word sol2[SIZE] = { 0xde79400f, 0x0a4022ca, 0xf8838980, 0x7e692567, 0xe6d1483f, 0x74e07c63, 0xf9618d41, 0x0e73523a };

    mod_sub(a1, b1, n1, result);
    assert(compareWordArrays(sol1, result, SIZE));
    printf("Test %u - Modular subtraction passed.\n", (*nbTest)++);

    mod_sub(a2, b2, n2, result);
    assert(compareWordArrays(sol2, result, SIZE));
    printf("Test %u - Modular subtraction passed.\n", (*nbTest)++);
}

void test_montgomery(word *nbTest) {
    word result[SIZE];

	word a1[SIZE] = { 0xb62d242a, 0xdd4afa1c, 0x9fb78238, 0xa2822926, 0x7163b897, 0xd578ac15, 0xd9f3ad89, 0x8818715a };
	word b1[SIZE] = { 0x978e5808, 0x99429782, 0xfe9f9216, 0x34d3df1b, 0x2c2837d6, 0xab448628, 0x08eefb1f, 0x8704715a };
	word n1[SIZE] = { 0x5b73ded1, 0x5d5774c6, 0x0dff3e9b, 0x57a9f080, 0x0b776e00, 0x63de11c9, 0xf8c9459a, 0x8d094351 };
	word n_prime1[SIZE] = { 0x5b58c5cf, 0xa1dfb6c3, 0x529f120c, 0x00336e8c, 0x786ec069, 0xd5c85ba7, 0xda43566f, 0xa9172637 };
	word sol1[SIZE] = { 0x0c8caec8, 0x826075df, 0x028819c2, 0x9229ec6b, 0x585b8f81, 0xb7a117b2, 0xce83c8ff, 0x1c07dbe0 };

	word a2[SIZE] = { 0xb69f3918, 0xcef7e454, 0xb09207a8, 0x7700818a, 0x5b1822c1, 0x92663375, 0xb7d2d0d9, 0x8b6192a3 };
	word b2[SIZE] = { 0x73dadc04, 0x8c9b701f, 0x89e6687c, 0xe25547dc, 0x4787b1c1, 0x126122a8, 0x07f28f2b, 0x8bd32438 };
	word n2[SIZE] = { 0x7b1e181b, 0xbccc0cf6, 0x6fbf3eb1, 0x3aec91eb, 0xb76932c3, 0x5a860a36, 0xbd467a5f, 0x90c3ca46 };
	word n_prime2[SIZE] = { 0xed03fded, 0x6f34e6e8, 0x62299597, 0xe81bbdc2, 0x2f984dca, 0xd2570255, 0x7304ae39, 0xd56a8a15 };
	word sol2[SIZE] = { 0xdd33720c, 0xdfa0a63b, 0x32193786, 0x3b32601a, 0x3cd6de8c, 0x0f4f10f4, 0xd823b958, 0x6c086cd0 };

    montMul(a1, b1, n1, n_prime1, result);
    assert(compareWordArrays(sol1, result, SIZE));

	montMul(a2, b2, n2, n_prime2, result);
	assert(compareWordArrays(sol2, result, SIZE));

    printf("Test %u - Montgomery multiplication passed.\n", (*nbTest)++);
}

void test_mod_inv(word *nbTest) {
    word res[SIZE];

	word x1[SIZE] = { 0x8a7d825f, 0x84f5fb21, 0xb24af02b, 0x3617511f, 0x0e92674d, 0x007da294, 0xea2f8a27, 0xb9adb54a };
	word p1[SIZE] = { 0x31590b3b, 0x1babb4ec, 0xb014f16d, 0x224b8a4e, 0x6c892361, 0xfad01644, 0xc0327f5e, 0xe003820a };
	word sol1[SIZE] = { 0x337c0954, 0x544e4214, 0x37fa269f, 0x8f605a20, 0xca1397c8, 0xb97cac9c, 0x4a9b9be4, 0x55c8b17e };

	word x2[SIZE] = { 0x6e53ca1a, 0x87996b0d, 0x60d7e55e, 0x74faef46, 0x029c0a27, 0x7832738c, 0x69e4079e, 0x8d1ff023 };
	word p2[SIZE] = { 0xb19a8453, 0xbe82056b, 0x25ef49a3, 0x5f78f219, 0x1ac9c8aa, 0x16943cee, 0x9a7c5ac1, 0x9aace62c };
	word sol2[SIZE] = { 0xbb3e33ad, 0xd0765f60, 0xdb3a76b3, 0x2c744cf3, 0xae7bdfe3, 0x603091e4, 0xf19ad4ee, 0x5fc52684 };
	
	word x3[SIZE] = { 0xf78b7379, 0x0c96294f, 0x20bdc766, 0x5799b03f, 0x4ddeea5b, 0x0edd4035, 0x84424bbf, 0x819e5c1a };
	word p3[SIZE] = { 0x2ed3941d, 0xc6a3cf68, 0x2b3cb385, 0x21510604, 0x105a3b7f, 0x898b648b, 0xe42200d2, 0x84628a48 };
	word sol3[SIZE] = { 0xd5de3db0, 0x3ef6b5f9, 0x2dab955d, 0xae29f5c3, 0x040d4734, 0xd1732fef, 0x9d58b1ba, 0x834cd5e1 };

    mod_inv(x1, p1, res);
    assert(compareWordArrays(sol1, res, SIZE));
    mod_inv(x2, p2, res);
    assert(compareWordArrays(sol2, res, SIZE));
    mod_inv(x3, p3, res);
    assert(compareWordArrays(sol3, res, SIZE));

    printf("Test %u - Modular inversion passed.\n", (*nbTest)++);
}

void test_mod_arithmetic(word *nbTest) {
    test_mod_add(nbTest);
    test_mod_sub(nbTest);
    test_montgomery(nbTest);
    test_mod_inv(nbTest);
}