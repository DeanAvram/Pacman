#include "MonstersAfterPacmanState.h"
#include <iostream>



MonstersAfterPacmanState::MonstersAfterPacmanState()
{
	OnStateEnter();
}


MonstersAfterPacmanState::~MonstersAfterPacmanState()
{
}


void MonstersAfterPacmanState::OnStateEnter()
{
	printf("MONSTERS CHASE PACMAN\n");
}

void MonstersAfterPacmanState::OnStateExit()
{
	printf("exiting state......");

}
