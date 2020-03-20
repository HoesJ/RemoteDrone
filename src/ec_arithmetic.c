#include "./../include/ec_arithmetic.h"
#include "./../include/mod_arithmetic.h"
#include "./../include/ec_parameters.h"
#include "./../include/compare_arrays.h"

/**
 * Checks whether the given point is on the elliptic curve. Returns 1 if
 * the given point is on the curve, 0 otherwise. The coordinates should be
 * given in the normal domain.
 */
word isOnCurve(const word *x, const word *y) {
    word x_mont[SIZE], y_mont[SIZE];
    word y_2[SIZE + 1], ax[SIZE + 1], x_3[SIZE + 1], RHS[SIZE + 1];
    
    /* Point at infinity is represented as both zero coordinates. */
    if (compareArrays(x, zero) && compareArrays(y, zero))
        return 1;

    montMul(x, r_2, p, p_prime, x_mont);
    montMul(y, r_2, p, p_prime, y_mont);

    /* Check if curve equation is satisfied. */
    montMul(y_mont, y_mont, p, p_prime, y_2);
    montMul(a_mont, x_mont, p, p_prime, ax);
    montMul(x_mont, x_mont, p, p_prime, x_3);    
    montMul(x_3, x_mont, p, p_prime, x_3);
    mod_add(x_3, ax, p, RHS);
    mod_add(RHS, b_mont, p, RHS);

    return compareArrays(y_2, RHS);
}

/**
 * Convert the given x and y to Jacobian coordinates. The parameters should be
 * given in the normal domain. The result is computed in the Montgomery domain.
 */
void toJacobian(const word *x, const word *y, const word *p, const word *p_prime, word *X, word *Y, word *Z) {
    montMul(x, r_2, p, p_prime, X);
    montMul(y, r_2, p, p_prime, Y);
    memcpy(Z, one_mont, SIZE * sizeof(word));
}

/**
 * Convert the given X, Y and Z to cartesian coordinates. The parameters should be
 * given in the Montgomery domain. The result is computed in the normal domain.
 */
void toCartesian(const word *X, const word *Y, const word *Z, const word *p, const word *p_prime, word *x, word *y) {
    word Z_2[SIZE], Z_3[SIZE];

    /* Compute Z_2 and Z_3. */
    montMul(Z, Z, p, p_prime, Z_2);
    montMul(Z_2, Z, p, p_prime, Z_3);
    
    /* Return to normal domain. */
    montMul(Z_2, one, p, p_prime, Z_2);
    montMul(Z_3, one, p, p_prime, Z_3);

    /* Divide by Z_2 and return to normal domain. */
    mod_inv(Z_2, p, Z_2);
    montMul(X, Z_2, p, p_prime, x);

    /* Divide by Z_3 and return to normal domain. */
    mod_inv(Z_3, p, Z_3);
    montMul(Y, Z_3, p, p_prime, y);
}

/**
 * Compute the doubling of the given point (X, Y, Z). All coordinates are in the Montgomery domain.
 */
void pointDouble(const word *X, const word *Y, const word *Z, const word *p, const word *p_prime, word *X_res, word *Y_res, word *Z_res) {
    word Y_2[SIZE], S[SIZE], M[SIZE], tmp1[SIZE], tmp2[SIZE];
    
    if (compareArrays(Y, zero)) {
        memcpy(X_res, zero, SIZE * sizeof(word));
        memcpy(Y_res, zero, SIZE * sizeof(word));
        memcpy(Z_res, one_mont, SIZE * sizeof(word));
    }

    /* Compute S. */
    montMul(Y, Y, p, p_prime, Y_2);
    montMul(Y_2, X, p, p_prime, S);
    mod_add(S, S, p, S);
    mod_add(S, S, p, S);

    /* Compute M. */
    montMul(Z, Z, p, p_prime, tmp1);
    montMul(tmp1, tmp1, p, p_prime, tmp1);
    montMul(a_mont, tmp1, p, p_prime, tmp1);
    montMul(X, X, p, p_prime, tmp2);
    mod_add(tmp2, tmp2, p, M);
    mod_add(M, tmp2, p, M);
    mod_add(M, tmp1, p, M);

    /* Compute X_res. */
    montMul(M, M, p, p_prime, tmp1);
    mod_add(S, S, p, tmp2);
    mod_sub(tmp1, tmp2, p, X_res);

    /* Compute Z_res. */
    montMul(Y, Z, p, p_prime, tmp1);
    mod_add(tmp1, tmp1, p, Z_res);

    /* Compute Y_res. */
    mod_sub(S, X_res, p, tmp1);
    montMul(M, tmp1, p, p_prime, tmp1);
    montMul(Y_2, Y_2, p, p_prime, tmp2);
    mod_add(tmp2, tmp2, p, tmp2);
    mod_add(tmp2, tmp2, p, tmp2);
    mod_add(tmp2, tmp2, p, tmp2);
    mod_sub(tmp1, tmp2, p, Y_res);
}

/**
 * Compute the addition of the given points (X1, Y1, Z1) and (X2, Y2, Z2). All coordinates are in the Montgomery domain.
 */
void pointAdd(const word *X1, const word *Y1, const word *Z1, const word *X2, const word *Y2, const word *Z2, const word *p,
              const word *p_prime, word *X_res, word *Y_res, word *Z_res) {
    word U1[SIZE], U2[SIZE], S1[SIZE], S2[SIZE], H_2[SIZE], H_3[SIZE];
    word *H, *R;

    /* Compute U1/S1 .*/
    montMul(Z2, Z2, p, p_prime, U1);
    montMul(U1, Z2, p, p_prime, S1);
    montMul(S1, Y1, p, p_prime, S1);
    montMul(U1, X1, p, p_prime, U1);

    /* Compute U2/S2 .*/
    montMul(Z1, Z1, p, p_prime, U2);
    montMul(U2, Z1, p, p_prime, S2);
    montMul(S2, Y2, p, p_prime, S2);
    montMul(U2, X2, p, p_prime, U2);

    if (compareArrays(U1, U2)) {
        if (compareArrays(S1, S2))
            pointDouble(X1, Y1, Z1, p, p_prime, X_res, Y_res, Z_res);
        else {
            memcpy(X_res, zero, SIZE * sizeof(word));
            memcpy(Y_res, zero, SIZE * sizeof(word));
            memcpy(Z_res, one_mont, SIZE * sizeof(word));
        }
    }

    /* Reuse variables. */
    H = U2;
    R = S2;

    /* Compute H/H_2/H_3/R. */
    mod_sub(U2, U1, p, H);
    montMul(H, H, p, p_prime, H_2);
    montMul(H_2, H, p, p_prime, H_3);
    mod_sub(S2, S1, p, R);

    /* Compute X_res. */
    montMul(R, R, p, p_prime, X_res);
    mod_sub(X_res, H_3, p, X_res);
    montMul(U1, H_2, p, p_prime, Y_res);
    mod_add(Y_res, Y_res, p, Y_res);
    mod_sub(X_res, Y_res, p, X_res);

    /* Compute Y_res. */
    montMul(U1, H_2, p, p_prime, Y_res);
    mod_sub(Y_res, X_res, p, Y_res);
    montMul(R, Y_res, p, p_prime, Y_res);
    montMul(S1, H_3, p, p_prime, H_3);
    mod_sub(Y_res, H_3, p, Y_res);

    /* Compute Z_res. */
    montMul(Z1, Z2, p, p_prime, Z_res);
    montMul(H, Z_res, p, p_prime, Z_res);
}