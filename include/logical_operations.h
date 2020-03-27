#ifndef LOGICAL_OPERATIONS_H_
#define LOGICAL_OPERATIONS_H_

/**
 * Calculates the XOR of two word arrays of both size elements.
 * Warning: This is a macro that needs you to declare i yourself!
 */
#define XOR128(res, a, b) {												\
((uint64_t*)(res))[0] = ((uint64_t*)(a))[0] ^ ((uint64_t*)(b))[0];		\
((uint64_t*)(res))[1] = ((uint64_t*)(a))[1] ^ ((uint64_t*)(b))[1];		\
}

/**
 * Calculates the AND of two word arrays of both size elements.
 * Warning: This is a macro that needs you to declare i yourself!
 */
#define AND128(res, a, b) {												\
((uint64_t*)(res))[0] = ((uint64_t*)(a))[0] & ((uint64_t*)(b))[0];		\
((uint64_t*)(res))[1] = ((uint64_t*)(a))[1] & ((uint64_t*)(b))[1];		\
}

#endif