#include "./../include/main_base_station.h"

int main_base_station(int txPipe, int rxPipe) {
    struct IO_ctx state;
    char message[32] = "first message from base station";
    char private[33] = "second message from base station";

    init_IO_ctx(&state, txPipe, rxPipe);

    /* Send test message */
    printf("BS - sending test message\n");
    send(&state, message, 31, 1);
    send(&state, private, 32, 1);

    printf("BS - done!\n");

    return 0;
}

/*void initializeBaseStation(struct SessionInfo* session, int* txPipe[2], int* rxPipe[2]) {
    /* Initialize state */
    /*session->state.systemState = Idle;
    session->state.kepState = Idle;
    session->state.commState = Idle;
    session->state.stateState = Idle;
    session->state.feedState = Idle;
    
    /* Initialize IO ctx */
    /*/*init_IO_ctx(&session->IO, txPipe, rxPipe);

    /* Initialize KEP ctx */
    /*init_KEP_ctx(&session->kep);

    /* Initialize session key */

    /* Initialize sequence NB */
    /*getRandomBytes(sizeof(word), &session->sequenceNb);

    /* Initialize target ID */
    /*session->targetID = 1;
}

void loop()
{
    /* Declare state */
    /*struct State state;

    /* Declare exteral signals */
    /*word start, quit, sendCommand;

    state.systemState = Idle;
    state.kepState = Idle;
    state.commState = Idle;
    state.stateState = Idle;
    state.feedState = Idle;

    while (1)
    {
        /* TODO: check external signals */
        /*start = 1;
        quit = 0;
        sendCommand = 0;

        /* Update state */
       /* state = stateMachine(state, start, quit);
    }
}

systemState stateMachine(struct State *currentState, word start, word quit)
{
    switch (currentState->systemState)
    {
    case Idle:
        if (start)
            currentState->systemState = KEP;
        else
            currentState->systemState = Idle;
        break;

    case KEP:
        if (!quit)
            currentState->kepState = kepContinue(currentState->kepState);
        else
            currentState->systemState = ClearSession;
        break;

    default:
        break;
    }
}
*/