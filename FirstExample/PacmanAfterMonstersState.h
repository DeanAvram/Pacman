#pragma once
#include "State.h"



class PacmanAfterMonstersState :
	public State
{

public:
	bool isPacmanChasing=true;

	PacmanAfterMonstersState();
	~PacmanAfterMonstersState();
	State* MakeTransition();
	void OnStateEnter();
	void OnStateExit();

};

