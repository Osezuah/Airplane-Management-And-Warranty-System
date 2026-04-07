#include "StateMachine.h"

ServerState StateMachine::GetState() const
{
    return currentState;
}

bool StateMachine::IsStateTransitionValid(ServerState state)
{
    switch (currentState) {
    case ServerState::IDLE:
        if (state == ServerState::WAITING_FOR_VERIFICATION)
            return true;
        break;

    case ServerState::WAITING_FOR_VERIFICATION:
        if (state == ServerState::CONNECTED)
            return true;
        break;

    case ServerState::CONNECTED:
        if (state == ServerState::PROCESSING_REQUEST)
            return true;
        if (state == ServerState::IDLE)
            return true;
        break;

    case ServerState::PROCESSING_REQUEST:
        if (state == ServerState::TRANSFERRING_DATA)
            return true;
        if (state == ServerState::CONNECTED)
            return true;
        break;

    case ServerState::TRANSFERRING_DATA:
        if (state == ServerState::CONNECTED)
            return true;
        break;

    case ServerState::ERROR_STATE:
        if (state == ServerState::IDLE)
            return true;
        break;
    default:
        return false;
    }
}

bool StateMachine::TransitionStateTo(ServerState newState)
{
    bool validTransition = IsStateTransitionValid(newState);

    if (validTransition || newState == ServerState::ERROR_STATE) {
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
    case ServerState::ERROR_STATE: return "ERROR_STATE";
    default: return "UNKNOWN";
    }
}
