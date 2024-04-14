#include "MonstersAfterPacmanState.h"
#include "PacmanAfterMonstersState.h"
#include <iostream>



MonstersAfterPacmanState::MonstersAfterPacmanState()
{
	OnStateEnter();
}


MonstersAfterPacmanState::~MonstersAfterPacmanState()
{
}

State* MonstersAfterPacmanState::MakeTransition()
{
	OnStateExit(); // set the relevant bool flags to false
	return new PacmanAfterMonstersState();
}

void MonstersAfterPacmanState::OnStateEnter()
{
	printf("MONSTERS CHASE PACMAN\n");
}

void MonstersAfterPacmanState::OnStateExit()
{
	printf("exiting state......");

}
