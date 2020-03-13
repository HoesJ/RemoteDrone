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
    signed_word i;

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
            sum      = (uint64_t) a[j] * b[i] + t[i + j] + C;
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

/** 
 * Compares two numbers a and b
 *  if (a > b)  returns 1
 *  if (a < b)  returns -1
 *  if (a == b) returns 0
 */
word compare(word* a, word* b) {
    signed_word i;
    for (i = SIZE -1; i >= 0; i--)
    {
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
void divideByTwo(word* a, word initialCarry) {
    signed_word i;
    word curr_carry = initialCarry;
    word next_carry = 0;

    for (i = SIZE - 1; i >= 0; i--) {
        next_carry = a[i] & 1;
        a[i] >>= 1;
        if (curr_carry)
            a[i] &= 1 << ((sizeof(word) * 8) - 1);
        curr_carry = next_carry; 
    }
}

/** 
 * Calcultes x^-1 mod p.
 * The result is written in inv.
 */
void mod_inv(word *x, word *p, word *inv) {
    word U[SIZE];
    word V[SIZE];
    word R[SIZE] = {0};
    word S[SIZE] = {0};
    word zero[SIZE] = {0};
    word carry;

    S[0] = 1;
    memcpy(U, p, SIZE * sizeof(word));
    memcpy(V, x, SIZE * sizeof(word));

    while (compare(V,zero)) {
        if ((U[0] & 1) == 0) {
            divideByTwo(U,0);
            carry = 0;
            if ((R[0] & 1) == 1)
                carry = add_overflow(R, p, R);
            divideByTwo(R, carry);
        } 
        else if ((V[0] & 1) == 0) {
            divideByTwo(V,0);
            carry = 0;
            if ((S[0] & 1) == 1)
                carry = add_overflow(S, p, S);
            divideByTwo(S, carry);
        } 
        else {
            if (compare(U,V) == 1) {
                sub_overflow(U, V, U);
                mod_sub(R, S, p, R);
            } else {
                sub_overflow(V, U, V);
                mod_sub(S, R, p, S);
            }
        }
    }
    if (U[0] > 1)
        memcpy(inv, zero, SIZE * sizeof(word));
    else if (compare(R,p) == 1)
        sub_overflow(R, p, inv);
    else
        memcpy(inv, R, SIZE * sizeof(word));
}
/* 
Reference:
Hars, Laszlo. (2006). Modular Inverse Algorithms Without Multiplications for Cryptographic Applications. 
EURASIP Journal on Embedded Systems. 2006. 032192. 10.1186/1687-3963-2006-032192. */
