#include "arithmetic.h"

/* Calculates res = a + b and returns the carry. */
word add_overflow(word *a, word *b, word *res, word length) {
    word carry = 0;

    /* Calculate the sum of a and b, starting at the LSB. */
    for (word i = 0; i < length; i++) {
        word temp = a[i] + carry;
        /* If the result is less than either of the operands, there is overflow
           and the carry for the next iteration needs to be set to 1. This carry
           adds an extra value of 1 to the next number in the result. */
        if (temp < a[i])
            carry = 1;
        else
            carry = 0;

        res[i] = temp + b[i];
        /* If the result is less than either of the operands, there is overflow.
           Note that overflow can occur only once and never twice per iteration,
           so the possible values of the carry are only 0 and 1. */
        if (res[i] < temp)
            carry = 1;
    }

    return carry;
}

/* Calculates res = a - b and returns the carry. */
word sub_overflow(word *a, word *b, word *res,
                         word length) {
    word carry = 0;

    /* Calculate the difference between a and b, starting at the LSB. */
    for (word i = 0; i < length; i++) {
        word temp = a[i] + carry;
        /* If the result is greater than its first operand, there is overflow
           and the carry for the next iteration needs to be set to 0xFFFFFFFF.
           This carry subtracts an extra value of 1 from the next number in
           the result, therefore it can be seen as a 'borrow'. */
        if (temp > a[i])
            carry = 0xFFFFFFFF;
        else
            carry = 0;

        res[i] = temp - b[i];
        /* If the result is greater than its first operand, there is overflow.
           Note that overflow can occur only once and never twice per iteration,
           so the possible values of the carry are only 0 and 0xFFFFFFFF. */
        if (res[i] > temp)
            carry = 0xFFFFFFFF;
    }

    return carry;
}

/* Calculates res = (a + b) mod N. */
void mod_add(volatile word *a, word *b, word *N, volatile word *res, word length) {
    if (add_overflow(a, b, res, length)) {
        /* If there is overflow, the result is greater than N and the value of N
           needs to be subtracted once, since a and b are both less than N. */
        sub_overflow(res, N, res, length);
        return;
    }

    /* If the result is greater than or equal to N, the value of N needs to be
       subtracted once, since a and b are both less than N. */
    for (word i = length - 1; i >= 0; i--) {
        if (res[i] < N[i])
            return;
        else if ((res[i] > N[i]) || (i == 0)) {
            sub_overflow(res, N, res, length);
            return;
        }
    }
}

/* Calculates res = (a - b) mod N. */
void mod_sub(word *a, word *b, word *N, word *res, word length) {
    if (sub_overflow(a, b, res, length))
        add_overflow(res, N, res, length);
}



/* Montgomery multiplication. */

/* Adds C to t, starting at index i. */
void add(word *t, word i, word C) {
    while (C != 0) {
        uint64_t sum = (uint64_t) t[i] + C;
        C            = (word)(sum >> 32);
        t[i]         = sum;
        i++;
    }
}

/* Performs conditional subtract of multiplication algorithm. The result is stored
   in a. Note that a has (size + 1) elements whereas b has only size elements. */
void conditionalSubtract(word *a, word *b, word size) {
	/* Only subtract if a >= b. */
	if (!a[size]) {
		for (word i = size - 1; i >= 0; i--) {
			if (a[i] > b[i]) break;
			if (a[i] < b[i]) return;
		}
	} else
		a[size] = 0;

	/* Do subtraction. */
	sub_overflow(a, b, a, size);
}

/* Calculates res = a * b * r^(-1) mod n.
   a, b, n, n_prime represent operands of size elements.
   res has (size + 1) elements. */
void montMul(word *a, word *b, word *n, word *n_prime, word *res, word size) {

    /* ------------------------------------------------------------------ //
    //                       Step 0: Initialization                       //
    // ------------------------------------------------------------------ */

    /* 32-bit carry. */
    word C;
    /* 32-bit sum. */
    word S;
    /* 64-bit temporary sum. */
    uint64_t sum;

    /* 65 x 32-bit value for storing a * b. */
    word t[2 * size + 1];
    /* Magic value z = (a * b) * n’ mod r. */
    word z;

    /* Initialize t to zero. */
    for (word i = 0; i < 2 * size + 1; i++)
        t[i] = 0;

    /* ------------------------------------------------------------------ //
    //                       Step 1: Multiplication                       //
    // ------------------------------------------------------------------ */

    for (word i = 0; i < size; i++) {
        C = 0;
        for (word j = 0; j < size; j++) {
            sum      = (uint64_t) a[j] * b[i] + t[i + j] + C;

            C        = (word)(sum >> 32);
            S        = (word) sum;
            t[i + j] = S;
        }
        t[i + size] = C;
    }

    /* ------------------------------------------------------------------ //
    //                         Step 2: Reduction                          //
    // ------------------------------------------------------------------ */

    for (word i = 0; i < size; i++) {
        C = 0;
        z = (uint64_t) t[i] * n_prime[0];

        for (word j = 0; j < size; j++) {
            sum      = (uint64_t) z * n[j] + t[i + j] + C;
            C        = (word)(sum >> 32);
            S        = (word)(sum);
            t[i + j] = S;
        }
        add(t, i + size, C);
    }
    for (word j = 0; j <= size; j++) {
        res[j] = t[j + size];
    }

    /* ------------------------------------------------------------------ //
    //                  Step 3: Conditional Subtraction                   //
    // ------------------------------------------------------------------ */

	conditionalSubtract(res, n, size);
}