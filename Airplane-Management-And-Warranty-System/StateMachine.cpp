#include "StateMachine.h"

ServerState StateMachine::GetState() const
{
    return currentState;
}

bool StateMachine::TransitionStateTo(ServerState newState)
{
    bool validTransition = false;

    switch (currentState) {
    case ServerState::IDLE:
        if (newState == ServerState::WAITING_FOR_VERIFICATION) 
            validTransition = true;
        break;

    case ServerState::WAITING_FOR_VERIFICATION:
        if (newState == ServerState::CONNECTED)
            validTransition = true;
        break;

    case ServerState::CONNECTED:
        if (newState == ServerState::PROCESSING_REQUEST)
            validTransition = true;
        if (newState == ServerState::IDLE)
            validTransition = true;
        break;

    case ServerState::PROCESSING_REQUEST:
        if (newState == ServerState::TRANSFERRING_DATA)
            validTransition = true;
        if (newState == ServerState::CONNECTED)
            validTransition = true;
        break;

    case ServerState::TRANSFERRING_DATA:
        if (newState == ServerState::CONNECTED)
            validTransition = true;
        break;

    case ServerState::ERROR:
        if (newState == ServerState::IDLE)
            validTransition = true;
        break;
    }

    if (validTransition || newState == ServerState::ERROR) {
        currentState = newState;
        return true;
    }

    return false;
}

std::string StateMachine::StateToString()
{
    switch (currentState) {
    case ServerState::IDLE: return "IDLE";
    case ServerState::WAITING_FOR_VERIFICATION: return "WAITING_FOR_VERIFICATION";
    case ServerState::CONNECTED: return "CONNECTED";
    case ServerState::PROCESSING_REQUEST: return "PROCESSING_REQUEST";
    case ServerState::TRANSFERRING_DATA: return "TRANSFERRING_DATA";
    case ServerState::ERROR: return "ERROR";
    default: return "UNKNOWN";
    }
}
