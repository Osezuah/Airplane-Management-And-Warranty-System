#pragma once
#include <string>

enum class ServerState {
	IDLE,
	WAITING_FOR_VERIFICATION,
	CONNECTED,
	PROCESSING_REQUEST,
	TRANSFERRING_DATA,
	ERROR_STATE
};

class StateMachine
{
	ServerState currentState = ServerState::IDLE;
public:
	ServerState GetState() const;

	bool IsStateTransitionValid(ServerState state);

	// Returns true on successful transition,
	// otherwise return false.
	bool TransitionStateTo(ServerState newState);

	std::string StateToString();
};

