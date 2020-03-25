#include "./../include/main_base_station.h"
#include "./../include/bs_kep_sm.h"
#include "./../include/bs_comm_sm.h"
#include "./../include/bs_state_sm.h"
#include "./../include/bs_feed_sm.h"

typedef enum
{
    Idle,
    ClearSession,
    KEP,
    SessionReady
} systemState;

struct State
{
    systemState systemState;
    kepState kepState;
    commState commState;
    stateState stateState;
    feedState feedState;
};


int main_base_station(int txPipe[2], int rxPipe[2])
{
    struct pipe_ctx pipeCtx;

    /* Create contex for sending and receiving */
    pipeCtx = bs_pipe_ctx_init(txPipe, rxPipe);

    /* start state machine */
    loop();
}

void loop()
{
    /* Declare state */
    struct State state;

    /* Declare exteral signals */
    word start, quit, sendCommand;

    state.systemState = Idle;
    state.kepState = Idle;
    state.commState = Idle;
    state.stateState = Idle;
    state.feedState = Idle;

    while (1)
    {
        /* TODO: check external signals */
        start = 1;
        quit = 0;
        sendCommand = 0;

        /* Update state */
        state = stateMachine(state, start, quit);
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
