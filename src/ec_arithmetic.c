#include "./../include/ec_arithmetic.h"
#include "./../include/mod_arithmetic.h"
#include "./../include/ec_parameters.h"
#include "./../include/compare_arrays.h"

/**
 * Checks whether the given point is on the elliptic curve. Returns 1 if
 * the given point is on the curve, 0 otherwise. The coordinates should be
 * given in the Montgomery domain.
 */
word isOnCurve(const word *x, const word *y) {
    word y_2[SIZE + 1], ax[SIZE + 1], x_3[SIZE + 1], RHS[SIZE + 1];

    /* Point at infinity is represented as both zero coordinates. */
    if (compareArrays(x, zero) && compareArrays(y, zero))
        return 1;

    /* Check if curve equation is satisfied. */
    montMul(y, y, p, p_prime, y_2);
    montMul(a_mont, x, p, p_prime, ax);
    montMul(x, x, p, p_prime, x_3);    
    montMul(x_3, x, p, p_prime, x_3);
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