#include "GLUT.h"
#include "CompareCells.h"
#include "Room.h"
#include <stdlib.h>
#include "Cell.h"
#include "State.h"
#include "MonstersAfterPacmanState.h"
#include "PacmanAfterMonstersState.h"
#include <vector>
#include <iostream>
#include <queue>
#include <time.h>
#include "Constants.h"
#include <windows.h>


using std::vector;
using std::priority_queue;

int maze[MSZ][MSZ] = { 0 };
Room rooms[NUM_OF_ROOMS];
vector <Cell> grays;
vector <Cell*> grays2;
vector <Cell> blacks;
vector <Cell*> monsterBlacks;
vector<Cell*> coins;
vector<Cell*> monsters;
priority_queue<Cell, vector<Cell>, CompareCells> pq;
bool AStar_is_running = false;
bool run = false;
int coinsRemained = NUM_OF_COINS;
int monsterRemained = NUM_OF_MONSTERS;
Cell* pacman;
vector<Cell*> path;
vector<Cell*> path_monster;
Cell* dangerousMonster;
int monsterGo = 0;
State* gameState;



void InitMaze();
void InitRooms();
void drawPlayers();
bool isInvadingRoom(int r, int c, int h, int w);
void connectRooms();
void Astar(Room r1, Room r2);
void drawPath(Cell* tmp);
void CheckNeighbor(int row, int col, Cell* p, int xt, int yt, vector <Cell>& grays, vector <Cell>& blacks);
void pacmanMove();
void EraseCoin(int x, int y);
void BFSIteration();
void monsterMove();
double DistanceMaze(int x1, int y1, int x2, int y2);
double DistanceForPackman(int x, int y);
bool PacmanCheckNeighbor(Cell* pCurrent, int row, int col);
void RestorePath();
void monsterAStar(int i);
double DistanceForMonster(int x, int y, int i);
bool CheckNeighborMonster(Cell* pCurrent, int x, int y, int i);
void RestorePathMonster(int i);
bool monsterDanger();




void init()
{
	srand(time(0)); // seed random numbers
	glClearColor(0, 0, 0.2, 0); // RGB - sets background color
	glOrtho(0, MSZ, 0, MSZ, -1, 1); // setup coordinate system
	InitMaze();
	//run = true;
}

void InitMaze()
{
	int i, j;

	// the border is WALL by default 

	for (i = 0; i < MSZ; i++)
	{
		for (j = 0; j < MSZ; j++)
		{
			maze[i][j] = WALL;
		}
	}
	InitRooms();
	connectRooms();
	drawPlayers();
	
}

void drawPlayers() {
	int pacmanX = rooms[0].get_cx(), pacmanY = rooms[0].get_cy();
	maze[pacmanX][pacmanY] = PACMAN;
	pacman = new Cell(pacmanX, pacmanY, nullptr);

	for (int i = 1; i <= NUM_OF_MONSTERS; i++) {
		int y = rooms[i].get_cy();
		int x = rooms[i].get_cx();
		maze[x][y] = MONSTER;
		monsters.push_back(new Cell(x, y, nullptr));

	}
	
	int addition,r,coinX,coinY;
	for (int i = 1; i <= NUM_OF_COINS; i++) {
		addition = (rand() % 4) + 1;
		if (i % 2 == 0) {
			addition = addition * -1;
		}
		r = rand() % NUM_OF_ROOMS;
		coinX = rooms[r].get_cx() + addition;
		coinY = rooms[r].get_cy() + addition;
		maze[coinX][coinY] = COIN;
		coins.push_back(new Cell(coinX, coinY, nullptr));
	}
}

void connectRooms() {
	for (int i = 0; i < NUM_OF_ROOMS; i++) {
		for (int j = 0; j < NUM_OF_ROOMS; j++) {
			if (i != j) {
				Astar(rooms[i], rooms[j]);
			}
		}
		printf("room %d connected \n", i);
	}
}

void runGame() {
    pacmanMove();
	Sleep(50);
	if (monsterGo) {
		monsterMove();
	}
	monsterGo = monsterGo ^ 1;
}


void monsterMove() {
	if (!run) { return; }
	vector<Cell*>::iterator ptr;
	int i = 0;
	for (ptr = monsters.begin(); ptr < monsters.end(); ptr++)
	{
			monsterAStar(i);
		
		Cell* pc;
		if (path_monster.size() >= 1)
		{
			pc = path_monster[path_monster.size() - 1];
			int row = pc->get_x();
			int col = pc->get_y();
			if (maze[row][col] == PACMAN)
			{
				run = false;
				printf(" MONSTERS WINS!!! ");
				maze[(*ptr)->get_x()][(*ptr)->get_y()] = SPACE;
				maze[row][col] = MONSTER;
				return;
			}
			maze[(*ptr)->get_x()][(*ptr)->get_y()] = SPACE;
			(*ptr)->set_y_val(col);
			(*ptr)->set_x_val(row);
			maze[row][col] = MONSTER;
			monsterBlacks.clear();
			path_monster.clear();
			i++;
		}
	}
}





void monsterAStar(int i)
{
	monsterBlacks.push_back(monsters[i]);
	Cell* pCurrent;
	vector<int> distances;
	bool found_path = false;
	int chances = 0;

	// 1. check if grays [queue] is not empty
	while (!found_path || chances > 4)
	{
		if (monsterBlacks.empty())
		{
			return;
		}
		else // grays is not empty
		{
			// 2. extract the FIRST element from grays and paint it BLACK
			pCurrent = *monsterBlacks.begin();

			// 2.1 paint it BLACK
			int row, col;
			// 2.2 and remove it from grays
			monsterBlacks.erase(monsterBlacks.begin());

			row = pCurrent->get_x();
			col = pCurrent->get_y();
			double directions[4] = { DistanceForMonster(row + 1, col, i),
									 DistanceForMonster(row - 1, col, i),
									 DistanceForMonster(row, col + 1, i),
									 DistanceForMonster(row, col - 1, i)
			};

			for (int f = 0; f < 4; f++) // check all directions in known order
			{
				int j = std::distance(directions, std::min_element(directions, directions + 4));
				directions[j] = DBL_MAX;
				if (!found_path)
				{
					switch (j)
					{
					case 0:// UP
						if (maze[row + 1][col] == SPACE || maze[row + 1][col] == PACMAN)
							found_path = CheckNeighborMonster(pCurrent, row + 1, col, i);
						break;
					case 1:	// DOWN
						if (!found_path)
							if (maze[row - 1][col] == SPACE || maze[row - 1][col] == PACMAN)
								found_path = CheckNeighborMonster(pCurrent, row - 1, col, i);
						break;
					case 2: // right		
						if (!found_path)
							if (maze[row][col + 1] == SPACE || maze[row][col + 1] == PACMAN)
								found_path = CheckNeighborMonster(pCurrent, row, col + 1, i);
						break;
					case 3:	// left		
						if (!found_path)
							if (maze[row][col - 1] == SPACE || maze[row][col - 1] == PACMAN)
								found_path = CheckNeighborMonster(pCurrent, row, col - 1, i);
						break;
					}
				}
			}
		}
		chances++;
	}

}


bool CheckNeighborMonster(Cell* pCurrent, int x, int y, int i)
{
	Cell* pc = new Cell(x, y, pCurrent);
	monsterBlacks.push_back(pc);
	if (DistanceMaze(x, y, monsters[i]->get_x(), monsters[i]->get_y()) >= 4 || maze[x][y]==PACMAN) // The algorithm is over
	{
		RestorePathMonster(i);
		return true;
	}
	return false;
}


void RestorePathMonster(int i)
{
	vector<Cell*>::iterator tmp;
	Cell* pc = new Cell();
	Cell* p = new Cell();
	double best_distance = DBL_MAX, tmp_dis;
	for (tmp = monsterBlacks.begin(); tmp != monsterBlacks.end(); tmp++)
	{
		tmp_dis = DistanceForMonster((*tmp)->get_x(), (*tmp)->get_y(), i);

		if (tmp_dis < best_distance)
		{
			best_distance = tmp_dis;
			pc = *tmp;
		}
	}
	p = pc->get_parent();
	while (p != nullptr)
	{
		path_monster.push_back(pc);
		pc = pc->get_parent();
		p = p->get_parent();
	}
}


double DistanceForMonster(int x, int y, int i)
{
	return DistanceMaze(x, y, pacman->get_x(), pacman->get_y());
}



double DistanceMaze(int x1, int y1, int x2, int y2)
{
	return sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
}

double DistanceForPackman(int x, int y)
{
	// distance from nearest coin(min)
	double dis_coin = DBL_MAX;
	double dis_tmp = 0;
	vector<Cell*>::iterator ptr;
	
		for (ptr = coins.begin(); ptr < coins.end(); ptr++)
		{
			dis_tmp = DistanceMaze(x, y, (*ptr)->get_x(), (*ptr)->get_y());
			if (dis_tmp < dis_coin)
				dis_coin = dis_tmp;
		}
		return abs(dis_coin);
	
	
}


bool monsterDanger() {
	for (int i = 0; i < monsters.size(); i++) {
		Cell* m = monsters[i];
		if (DistanceMaze(pacman->get_x(), pacman->get_y(), m->get_x(), m->get_y()) < 7) {
			dangerousMonster = monsters[i];
			return true;
		}
	}
	return false;
}

void pacmanMove()
{
	if (coinsRemained == 0)
	{
		run = false;
		printf(" PACMAN WINS!!!");
	}
	else if (run)
	{
		
	    BFSIteration();
		Cell* pc;
		pc = path[path.size() - 1];
		int row = pc->get_x();
		int col = pc->get_y();
		if (maze[row][col] == COIN)
		{
			coinsRemained--;
			EraseCoin(row, col);
		}
		maze[pacman->get_x()][pacman->get_y()] = SPACE;
		pacman->set_y_val(col);
		pacman->set_x_val(row);
		maze[row][col] = PACMAN;
		grays2.clear();
		path.clear();
	}

}



void BFSIteration()
{
	grays2.push_back(pacman);
	Cell* pCurrent;
	bool found_path = false;
	// 1. check if grays is not empty
	while (!found_path)
	{
		if (grays2.empty())
		{
			found_path = true;
			printf(" NO SOLUTION");
		}
		else // grays is not empty
		{
			// 2. extract the first element from grays and paint it BLACK
			pCurrent = *grays2.begin();
			int row, col;
			// 2. remove it from grays
			grays2.erase(grays2.begin());

			row = pCurrent->get_x();
			col = pCurrent->get_y();
			//maze[rowGray][colGray] = BLACK_START;
			// 3 check the neighbors of pCurrent and pick the white one and add them to the end of grays
			// UP
			if (run)
			{
				if (maze[row + 1][col] == SPACE || maze[row + 1][col] == COIN)
					found_path = PacmanCheckNeighbor(pCurrent, row + 1, col);
				if (!found_path)
					// DOWN
					if (maze[row - 1][col] == SPACE || maze[row - 1][col] == COIN)
						found_path = PacmanCheckNeighbor(pCurrent, row - 1, col);
				// right
				if (!found_path)
					if (maze[row][col + 1] == SPACE || maze[row][col + 1] == COIN)
						found_path = PacmanCheckNeighbor(pCurrent, row, col + 1);
				// left	
				if (!found_path)
					if (maze[row][col - 1] == SPACE || maze[row][col - 1] == COIN)
						found_path = PacmanCheckNeighbor(pCurrent, row, col - 1);
			}
		}
	}
}


bool PacmanCheckNeighbor(Cell* pCurrent, int row, int col)
{
	Cell* pc = new Cell(row, col, pCurrent);
	grays2.push_back(pc);
	if (DistanceMaze(row, col, pacman->get_x(), pacman->get_y()) >= 8 || maze[row][col]== COIN)
	{
		RestorePath();
		return true;
	}
	return false;
}


void RestorePath()
{
	    vector<Cell*>::iterator tmp;
	    Cell* pc = new Cell();
	    Cell* p = new Cell();
			if (!monsterDanger()) {
				double best_distance = DBL_MAX, tmp_dis;
				for (tmp = grays2.begin(); tmp != grays2.end(); tmp++)
				{
					tmp_dis = DistanceForPackman((*tmp)->get_x(), (*tmp)->get_y());

					if (tmp_dis < best_distance)
					{
						best_distance = tmp_dis;
						pc = *tmp;
					}
				}
			}
			else {
				double best_distance = 0, tmp_dis;
				for (tmp = grays2.begin(); tmp != grays2.end(); tmp++)
				{
					tmp_dis = DistanceMaze((*tmp)->get_x(), (*tmp)->get_y(), dangerousMonster->get_x(), dangerousMonster->get_y());

					if (tmp_dis > best_distance)
					{
						best_distance = tmp_dis;
						pc = *tmp;
					}
				}
			}
		
	    
	p = pc->get_parent();
	while (p != nullptr)
	{
		path.push_back(pc);
		pc = pc->get_parent();
		p = p->get_parent();
	}
	
}




void EraseCoin(int x, int y)
{
	vector<Cell*>::iterator ptr;
	for (ptr = coins.begin(); ptr < coins.end(); ptr++)
	{
		if ((*ptr)->get_x() == x && (*ptr)->get_y()== y)
		{
			coins.erase(ptr);
			return;
		}
	}
}

//void EraseMonster(int x, int y)
//{
//	vector<Cell*>::iterator ptr;
//	for (ptr = monsters.begin(); ptr < monsters.end(); ptr++)
//	{
//		if ((*ptr)->get_x() == x && (*ptr)->get_y()== y)
//		{
//			monsters.erase(ptr);
//			return;
//		}
//	}
//}



void Astar(Room r1, Room r2) {
	vector <Cell> grays;
	vector <Cell> blacks;
	Cell* pcurrent;
	int xTarget = r2.get_cx(), yTarget = r2.get_cy();
	Cell first = *(new Cell(r1.get_cx(), r1.get_cy(), nullptr, xTarget, yTarget, 0));
	pq.push(first);
	grays.push_back(first);
	vector<Cell>::iterator it_gray;
	bool AStar_is_running = true;

	while (AStar_is_running) {
		if (pq.empty())
		{
			//printf("no solution  ");
			AStar_is_running = false;
			return;
		}
		else {
			pcurrent = new Cell(pq.top());
			pq.pop();
			it_gray = find(grays.begin(), grays.end(), *pcurrent);
			if (it_gray == grays.end())
			{
				printf("pcurrent not found");
				AStar_is_running = false;
				return;
			}
			grays.erase(it_gray);
			blacks.push_back(*pcurrent);
			int r = pcurrent->get_x(), c = pcurrent->get_y();
			if (AStar_is_running && r < MSZ) {
				CheckNeighbor(r + 1, c, pcurrent, xTarget, yTarget, grays, blacks);
			}
			// down
			if (AStar_is_running && r > 0) {
				CheckNeighbor(r - 1, c, pcurrent, xTarget, yTarget, grays, blacks);
			}

			// left
			if (AStar_is_running && c > 0) {
				CheckNeighbor(r, c - 1, pcurrent, xTarget, yTarget, grays, blacks);
			}

			// right
			if (AStar_is_running && c < MSZ) {
				CheckNeighbor(r, c + 1, pcurrent, xTarget, yTarget, grays, blacks);
			}


		}
	}

}

void CheckNeighbor(int row, int col, Cell* p, int xt, int yt, vector <Cell>& grays, vector <Cell>& blacks) {
	double cost;
	vector<Cell>::iterator it_gray;
	vector<Cell>::iterator it_black;
	vector<Cell> tmp; //  we'll need it to update pq
	Cell* pc;
	if (maze[row][col] == SPACE) {
		cost = 1;
	}
	else {
		cost = 2.7;
	}
	// is maze[row][col] a TARGET
	if (xt == row && yt == col)
	{
		AStar_is_running = false;
		while (!pq.empty()) {
			pq.pop();
		}
		drawPath(p);
		return;
	}
	else
	{
		//Cell pneighbor = *(new Cell(row, col, p, xt, yt, p->get_g_val() + cost));
		Cell* ctmp = new Cell(row, col, p, xt, yt, p->get_g_val() + cost);
		//pq.push(pneighbor);
		it_gray = find(grays.begin(), grays.end(), *ctmp);
		it_black = find(blacks.begin(), blacks.end(), *ctmp);
		if (it_gray == grays.end() && it_black == blacks.end())//it is white
		{
			pq.push(*ctmp); // add it to pq
			grays.push_back(*ctmp); // and paint it gray
		}
		else if (it_gray != grays.end()) // it is gray
		{
			if (ctmp->get_g_val() < it_gray->get_g_val()) // we have to update G and F in grays and we have to update pq
			{
				*it_gray = *ctmp;

				// update pq;
				pc = new Cell(pq.top());
				while (!pq.empty() && !(pc->get_x() == ctmp->get_x() && pc->get_y() == ctmp->get_y()))
				{
					pq.pop();
					tmp.push_back(*pc);
					pc = new Cell(pq.top());
				}
				// if pq is not empty pn has been found
				pq.pop();
				pq.push(*ctmp); // insert to pq the better copy of the neighbor
				while (!tmp.empty())
				{
					pq.push(tmp[tmp.size() - 1]);
					tmp.pop_back();
				}
			}
		}
	}
}

void drawPath(Cell* tmp)
{
	while (tmp != nullptr)
	{
		maze[tmp->get_x()][tmp->get_y()] = SPACE;
		tmp = tmp->get_parent();
	}
}

void InitRooms() {
	int minimum_width = 10, minimum_height = 10, x, y, w, h, i;

	for (i = 0; i < NUM_OF_ROOMS; i++)
	{
		do
		{
			x = 7 + minimum_height / 2 + rand() % (MSZ - minimum_height - 16);
			y = 7 + minimum_width / 2 + rand() % (MSZ - minimum_width - 16);
			w = minimum_width - 2 + rand() % 20;
			h = minimum_height - 2 + rand() % 20;
		} while (isInvadingRoom(x, y, h, w));
		rooms[i] = *(new Room(x, y, h, w));
		rooms[i].fillRoom(maze, SPACE);
	}
}


bool isInvadingRoom(int r, int c, int h, int w)
{
	for (int i = r - h / 2 - 1; i <= r + h / 2 + 1; i++)
		for (int j = c - w / 2 - 1; j <= c + w / 2 + 1; j++)
			if (maze[i][j] == SPACE)
				return true;

	return false;
}

void DrawMaze()
{
	int i, j;

	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
		{
			// set color
			switch (maze[i][j])
			{
			case WALL:
				glColor3d(0.137255, 0.137255, 0.556863);// dark red
				break;
			case SPACE:
				glColor3d(1, 1, 1); // white
				break;
			case PACMAN:
				glColor3d(0, 1, 1); // white
				break;
			case MONSTER:
				glColor3d(0, 0, 0); // white
				break;
			case COIN:
				glColor3d(0.8, 0.498039, 0.196078);
				break;
			}
			
			glBegin(GL_POLYGON);
			glVertex2d(j, i);
			glVertex2d(j, i + 1);
			glVertex2d(j + 1, i + 1);
			glVertex2d(j + 1, i);
			glEnd();
		}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // fills all the frame buffer with background color

	DrawMaze();

	glutSwapBuffers();// show what was drawn in "frame buffer"
}

void idle()
{
	if (run) {
		runGame();
	}
	glutPostRedisplay(); // calls indirectly to display
}

void menu(int choice)
{
	if (choice == 1)
	{
		if (gameState == NULL) {
			gameState = new MonstersAfterPacmanState();
		}
		run = true;
	}
	if (choice == 2)
	{
		if (gameState == NULL) {
			gameState = new MonstersAfterPacmanState();
		}
		else {
			gameState=gameState->MakeTransition();
		}
		run = true;
	}
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE); // double means it uses double buffer
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(600, 100);
	glutCreateWindow("Pacman by Assaf Ariely & Vladi Hagay");

	glutDisplayFunc(display); // refresh function
	glutIdleFunc(idle); // runs all the time in the background

	glutCreateMenu(menu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutAddMenuEntry("Start Game", 1);
	glutAddMenuEntry("Change State", 2);

	init();

	glutMainLoop();
}