#include "./../include/mod_arithmetic.h"

/*
 * Calculates res = (a + b) and detects overflow.
 */
word add_overflow(const word *a, const word *b, word *res) {
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

/**
 * Calculates res = a - b and returns the carry.
 */
word sub_overflow(const word *a, const word *b, word *res) {
    word carry = 0;
    word temp;
    word i;

    /* Calculate the difference between a and b, starting at the LSB. */
    for (i = 0; i < SIZE; i++) {
        temp = a[i] + carry;
        /* If the result is greater than its first operand, there is overflow
           and the carry for the next iteration needs to be set to ALL_ONE_MASK.
           This carry subtracts an extra value of 1 from the next number in
           the result, therefore it can be seen as a 'borrow'. */
        if (temp > a[i])
            carry = ALL_ONE_MASK;
        else
            carry = 0;

        res[i] = temp - b[i];
        /* If the result is greater than its first operand, there is overflow.
           Note that overflow can occur only once and never twice per iteration,
           so the possible values of the carry are only 0 and ALL_ONE_MASK. */
        if (res[i] > temp)
            carry = ALL_ONE_MASK;
    }

    return carry;
}

/**
 * Calculates res = (a + b) mod N.
 */
void mod_add(const word *a, const word *b, const word *N, word *res) {
    word i;

    /* If there is overflow, the result is greater than N and the value of N
       needs to be subtracted once, since a and b are both less than N. */
    if (add_overflow(a, b, res)) {
        sub_overflow(res, N, res);
        return;
    }

    /* If the result is greater than or equal to N, the value of N needs to be
       subtracted once, since a and b are both less than N. */
    for (i = SIZE - 1;; i--) {
        if (res[i] < N[i])
            return;
        else if ((res[i] > N[i]) || (i == 0)) {
            sub_overflow(res, N, res);
            return;
        }
    }
}

/**
 * Calculates res = (a - b) mod N.
 */
void mod_sub(const word *a, const word *b, const word *N, word *res) {
    if (sub_overflow(a, b, res))
        add_overflow(res, N, res);
}



/* Montgomery multiplication. */

/**
 * Adds C to t, starting at index i.
 */
void add(word *t, word i, word C) {
    double_word sum;

    while (C != 0) {
        sum          = (double_word) t[i] + C;
        C            = (word)(sum >> BITS);
        t[i]         = sum;
        i++;
    }
}

/**
 * Performs conditional subtract of multiplication algorithm. The result is stored
 * in a. Note that a has (SIZE + 1) elements whereas b has only SIZE elements.
 */
void conditionalSubtract(word *a, const word *b) {
    word i;

	/* Only subtract if a >= b. */
	if (!a[SIZE]) {
		for (i = SIZE - 1;; i--) {
			if  (a[i] < b[i]) return;
            if ((a[i] > b[i]) || (i == 0)) break;
		}
	} else
		a[SIZE] = 0;

	/* Do subtraction. */
	sub_overflow(a, b, a);
}

/**
 * Calculates res = a * b * r^(-1) mod n.
 * a, b, n, n_prime represent operands of SIZE elements.
 * The result is stored in res which has SIZE elements.
 */
void montMul(const word *a, const word *b, const word *n, const word *n_prime, word *res) {

    /* ------------------------------------------------------------------ //
    //                       Step 0: Initialization                       //
    // ------------------------------------------------------------------ */

    /* BITS-bit carry. */
    word C;
    /* BITS-bit sum. */
    word S;
    /* (2 * BITS)-bit temporary sum. */
    double_word sum;

    /* (2 * SIZE + 1) x BITS-bit value for storing a * b. */
    word t[2 * SIZE + 1] = { 0 };
    /* Magic value z = (a * b) * n’ mod r. */
    word z;

    /* Loop variables. */
    word i, j;

    /* ------------------------------------------------------------------ //
    //                       Step 1: Multiplication                       //
    // ------------------------------------------------------------------ */

    for (i = 0; i < SIZE; i++) {
        C = 0;
        for (j = 0; j < SIZE; j++) {
            sum      = (double_word) a[j] * b[i] + t[i + j] + C;
            C        = (word)(sum >> BITS);
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
            C        = (word)(sum >> BITS);
            S        = (word)(sum);
            t[i + j] = S;
        }
        add(t, i + SIZE, C);
    }

    /* ------------------------------------------------------------------ //
    //                  Step 3: Conditional Subtraction                   //
    // ------------------------------------------------------------------ */

	conditionalSubtract(t + SIZE, n);

    /* Copy result to res. */
    memcpy(res, t + SIZE, SIZE * sizeof(word));
}



/* Modular inversion. */

/** 
 * Compares two numbers a and b:
 *  if (a > b)  returns 1.
 *  if (a < b)  returns -1.
 *  if (a == b) returns 0.
 */
word compare(const word *a, const word *b) {
    signed_word i;
    for (i = SIZE - 1; i >= 0; i--) {
        if (a[i] > b[i])
            return 1;
        else if (a[i] < b[i])
            return -1;        
    }
    return 0;    
}

/** 
 * Divides the given number by two. The number itself is changed.
 */
void divideByTwo(word *a, word initialCarry) {
    signed_word i;
    word curr_carry = initialCarry;
    word next_carry = 0;

    for (i = SIZE - 1; i >= 0; i--) {
        next_carry = a[i] & 1;
        a[i] >>= 1;
        if (curr_carry)
            a[i] |= 1 << ((sizeof(word) * 8) - 1);
        curr_carry = next_carry; 
    }
}

/** 
 * Calculates x^(-1) mod p.
 * The result is written in inv.
 */
void mod_inv(const word *x, const word *p, word *inv) {
    word U[SIZE];
    word V[SIZE];
    word R[SIZE] = {0};
    word S[SIZE] = {0};
    word zero[SIZE] = {0};
    word carry;

    S[0] = 1;
    memcpy(U, p, SIZE * sizeof(word));
    memcpy(V, x, SIZE * sizeof(word));

    while (compare(V, zero)) {
        if ((U[0] & 1) == 0) {
            divideByTwo(U, 0);
            carry = 0;
            if ((R[0] & 1) == 1)
                carry = add_overflow(R, p, R);
            divideByTwo(R, carry);
        } 
        else if ((V[0] & 1) == 0) {
            divideByTwo(V, 0);
            carry = 0;
            if ((S[0] & 1) == 1)
                carry = add_overflow(S, p, S);
            divideByTwo(S, carry);
        } 
        else {
            if (compare(U, V) == 1) {
                sub_overflow(U, V, U);
                mod_sub(R, S, p, R);
            }
            else {
                sub_overflow(V, U, V);
                mod_sub(S, R, p, S);
            }
        }
    }
    if (U[0] > 1)
        memcpy(inv, zero, SIZE * sizeof(word));
    else if (compare(R, p) == 1)
        sub_overflow(R, p, inv);
    else
        memcpy(inv, R, SIZE * sizeof(word));
}
/* Reference:
Hars, Laszlo. (2006). Modular Inverse Algorithms Without Multiplications for Cryptographic Applications. 
EURASIP Journal on Embedded Systems. 2006. 032192. 10.1186/1687-3963-2006-032192. */