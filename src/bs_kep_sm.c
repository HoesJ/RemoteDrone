#include "./../include/bs_kep_sm.h"

/*kepState kepContinue(kepState currentState, struct pipe_ctx* io) {
    switch (currentState)
    {
    case Idle:
        return KEP1_compute;
    
    case KEP1_compute:
        KEP1_compute_handler();
        return KEP1_send;

    default:
        break;
    }
}

word KEP1_compute_handler() {
    word X[SIZE], Y[SIZE], Z[SIZE];
    word scalar;

    ECDHGenerateRandomSample(&scalar, X, Y, Z);
    
}
*/