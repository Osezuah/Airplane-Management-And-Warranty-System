#pragma once
#include <string>

enum class ServerState {
	IDLE,
	WAITING_FOR_VERIFICATION,
	CONNECTED,
	PROCESSING_REQUEST,
	TRANSFERRING_DATA,
	ERROR
};

class StateMachine
{
	ServerState currentState = ServerState::IDLE;
public:
	ServerState GetState() const;

	// Returns true on successful transition,
	// otherwise return false.
	bool TransitionStateTo(ServerState newState);

	std::string StateToString();
};

