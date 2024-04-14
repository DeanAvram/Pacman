#pragma once
#include "State.h"



class MonstersAfterPacmanState :
	public State
{

public:
	bool isPacmanChasing=false;

	MonstersAfterPacmanState();
	~MonstersAfterPacmanState();
	State* MakeTransition();
	void OnStateEnter();
	void OnStateExit();

};

