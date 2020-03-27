#include "./../include/bs_kep_sm.h"
/*
kepState kepContinue(struct SessionInfo* session, kepState currentState) {
    switch (currentState)
    {
    case Idle:
        return KEP1_compute;
    
    case KEP1_compute:
        if (!KEP1_compute_handler(session))
            return KEP1_send;
        else
            return KEP1_compute;
    
    case KEP1_send:
        if(!KEP1_send_handler(session))
            return KEP1_wait;
        else
            return Idle; /* handler sets systemstate to clear session */

    /*default:
        break;
    }
}

word KEP1_compute_handler(struct SessionInfo* session) {
    word X[SIZE], Y[SIZE], Z[SIZE];

    ECDHGenerateRandomSample(session->kep.scalar, X, Y, Z);
    toCartesian(X, Y, Z, session->kep.generatedPointX, session->kep.generatedPointY);
    return 0;
}

word KEP1_send_handler(struct SessionInfo* session) {
    uint8_t buffer[KEP1_MESSAGE_BYTES];

    if (session->kep.numTransmission >= KEP_MAX_RETRANSMISSIONS) {
        /* Abort KEP */
        /*session->systemState = ClearSession;
        return 1;
    }

    /* Form message */
    /*buffer[0] = TYPE_KEP1_SEND;     /* TYPE */
    /*buffer[3] = KEP1_MESSAGE_BYTES; /* Length */
    /*/*buffer[11] = session->targetID; /* targetID */
/*
}

word KEP1_wait_handler(struct SessionInfo* session) {
    double_word  currentTime;
    double_word  elapsedTime;

    currentTime = (double_word)clock();
    elapsedTime = ((float_word)currentTime - session->kep.timeOfTransmission) / CLOCKS_PER_SEC;
    if (elapsedTime > KEP_RETRANSMISSION_TIMEOUT)
}
*/