#pragma once
// pure virtual class (interface)

class Player;
class State
{

public:

	State();
	~State();

	bool isPacmanChasing;

	virtual void OnStateEnter() = 0;
	virtual void OnStateExit() = 0;

};

