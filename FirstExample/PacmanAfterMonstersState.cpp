#include "MonstersAfterPacmanState.h"
#include "PacmanAfterMonstersState.h"
#include <iostream>



PacmanAfterMonstersState::PacmanAfterMonstersState()
{
	OnStateEnter();
}


PacmanAfterMonstersState::~PacmanAfterMonstersState()
{
}

State* PacmanAfterMonstersState::MakeTransition()
{
	OnStateExit(); // set the relevant bool flags to false
	return new MonstersAfterPacmanState();

}

void PacmanAfterMonstersState::OnStateEnter()
{
	printf("PACMAN CHASES MONSTERS\n");
}

void PacmanAfterMonstersState::OnStateExit()
{
	printf("exiting state......");
}
