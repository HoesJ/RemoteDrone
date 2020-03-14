#include "./../include/arithmetic.h"

/* Calculates res = a + b and returns the carry. */
word add_overflow(word *a, word *b, word *res) {
    word carry = 0;
    word temp;
    word i;

    /* Calculate the sum of a and b, starting at the LSB. */
    for (i = 0; i < SIZE; i++) {
        temp = a[i] + carry;
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
word sub_overflow(word *a, word *b, word *res) {
    word carry = 0;
    word temp;
    word i;

    /* Calculate the difference between a and b, starting at the LSB. */
    for (i = 0; i < SIZE; i++) {
        temp = a[i] + carry;
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
void mod_add(word *a, word *b, word *N, word *res) {
    word i;

    if (add_overflow(a, b, res)) {
        for (i = SIZE - 1; i >= 0; i--) {
            if (res[i] < N[i])
                return;
            else if ((res[i] > N[i]) || (i == 0)) {
                sub_overflow(res, N, res);
                return;
            }
        }
    }
}

/* Calculates res = (a - b) mod N. */
void mod_sub(word *a, word *b, word *N, word *res) {
    if (sub_overflow(a, b, res))
        add_overflow(res, N, res);
}



/* Montgomery multiplication. */

/* Adds C to t, starting at index i. */
void add(word *t, word i, word C) {
    double_word sum;

    while (C != 0) {
        sum          = (double_word) t[i] + C;
        C            = (word)(sum >> 32);
        t[i]         = sum;
        i++;
    }
}

/* Performs conditional subtract of multiplication algorithm. The result is stored
   in a. Note that a has (SIZE + 1) elements whereas b has only SIZE elements. */
void conditionalSubtract(word *a, word *b) {
    word i;

	/* Only subtract if a >= b. */
	if (!a[SIZE]) {
		for (i = SIZE - 1; i >= 0; i--) {
			if (a[i] > b[i]) break;
			if (a[i] < b[i]) return;
		}
	} else
		a[SIZE] = 0;

	/* Do subtraction. */
	sub_overflow(a, b, a);
}

/* Calculates res = a * b * r^(-1) mod n.
   a, b, n, n_prime represent operands of SIZE elements.
   res has (SIZE + 1) elements. */
void montMul(word *a, word *b, word *n, word *n_prime, word *res) {

    /* ------------------------------------------------------------------ //
    //                       Step 0: Initialization                       //
    // ------------------------------------------------------------------ */

    /* 32-bit carry. */
    word C;
    /* 32-bit sum. */
    word S;
    /* 64-bit temporary sum. */
    double_word sum;

    /* 65 x 32-bit value for storing a * b. */
    word t[2 * SIZE + 1];
    /* Magic value z = (a * b) * n’ mod r. */
    word z;

    /* Loop variables. */
    word i, j;

    /* Initialize t to zero. */
    for (i = 0; i < 2 * SIZE + 1; i++)
        t[i] = 0;

    /* ------------------------------------------------------------------ //
    //                       Step 1: Multiplication                       //
    // ------------------------------------------------------------------ */

    for (i = 0; i < SIZE; i++) {
        C = 0;
        for (j = 0; j < SIZE; j++) {
            sum      = (double_word) a[j] * b[i] + t[i + j] + C;

            C        = (word)(sum >> 32);
            S        = (word) sum;
            t[i + j] = S;
        }
        t[i + SIZE] = C;
    }

    /* ------------------------------------------------------------------ //
    //                         Step 2: Reduction                          //
    // ------------------------------------------------------------------ */

    for (i = 0; i < SIZE; i++) {
        C = 0;
        z = (double_word) t[i] * n_prime[0];

        for (j = 0; j < SIZE; j++) {
            sum      = (double_word) z * n[j] + t[i + j] + C;
            C        = (word)(sum >> 32);
            S        = (word)(sum);
            t[i + j] = S;
        }
        add(t, i + SIZE, C);
    }
    for (i = 0; i <= SIZE; i++) {
        res[i] = t[i + SIZE];
    }

    /* ------------------------------------------------------------------ //
    //                  Step 3: Conditional Subtraction                   //
    // ------------------------------------------------------------------ */

	conditionalSubtract(res, n);
}

void mod_inv(word *x, word *p, word *N, word *inv) {
    
}