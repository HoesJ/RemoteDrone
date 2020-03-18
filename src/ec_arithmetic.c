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

    montMul(y, y, p, p_prime, y_2);
    montMul(a_mont, x, p, p_prime, ax);
    montMul(x, x, p, p_prime, x_3);    
    montMul(x_3, x, p, p_prime, x_3);
    mod_add(x_3, ax, p, RHS);
    mod_add(RHS, b_mont, p, RHS);

    return compareArrays(y_2, RHS);
}