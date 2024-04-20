#include "GLUT.h"
#include "CompareCells.h"
#include "Room.h"
#include <stdlib.h>
#include "Cell.h"
#include "State.h"
#include "MonstersAfterPacmanState.h"
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
void RoomAstar(Room r1, Room r2);
void drawPath(Cell* tmp);
void RoomCheckNeighbor(int row, int col, Cell* p, int xt, int yt, vector <Cell>& grays, vector <Cell>& blacks);
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
void updatePriorityQueue(Cell& cell);




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
				RoomAstar(rooms[i], rooms[j]);
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
				printf("MONSTERS WINS!!!");
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
		if (DistanceMaze(pacman->get_x(), pacman->get_y(), m->get_x(), m->get_y()) < DANGER_DIST) {
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
		printf("PACMAN WINS!!!");
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
			printf("Pacman ate %d coins. %d left\n", (NUM_OF_COINS - coinsRemained) ,coinsRemained);
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
	while (!found_path)
	{
		if (grays2.empty())
		{
			found_path = true;
			printf("NO SOLUTION");
		}
		else
		{
			
			pCurrent = *grays2.begin();
			int row, col;
			// Grays is not empty. Get the first one and remove it from grays
			grays2.erase(grays2.begin());

			row = pCurrent->get_x();
			col = pCurrent->get_y();
			// check all the neighbors of pCurrent. Pick the white one and add them to the end of grays
			if (run)
			{
				// UP
				if (maze[row + 1][col] == SPACE || maze[row + 1][col] == COIN)
					found_path = PacmanCheckNeighbor(pCurrent, row + 1, col);
				// DOWN
				if (!found_path && (maze[row - 1][col] == SPACE || maze[row - 1][col] == COIN))
					found_path = PacmanCheckNeighbor(pCurrent, row - 1, col);
				// right
				if (!found_path && (maze[row][col + 1] == SPACE || maze[row][col + 1] == COIN))
					found_path = PacmanCheckNeighbor(pCurrent, row, col + 1);
				// left	
				if (!found_path && (maze[row][col - 1] == SPACE || maze[row][col - 1] == COIN))
					found_path = PacmanCheckNeighbor(pCurrent, row, col - 1);
			}
		}
	}
}


bool PacmanCheckNeighbor(Cell* pCurrent, int row, int col)
{
	Cell* pc = new Cell(row, col, pCurrent);
	grays2.push_back(pc);
	if (DistanceMaze(row, col, pacman->get_x(), pacman->get_y()) >= MONSTER_DEPTH || maze[row][col]== COIN)
	{
		RestorePath();
		return true;
	}
	return false;
}

void RestorePath()
{
	// Iterator for gray cells
	vector<Cell*>::iterator tmp;

	// Initialize a pointer for the current cell and a pointer for the parent cell
	Cell* pc = nullptr;
	Cell* p = nullptr;

	// Determine the best cell to start restoring the path
	if (!monsterDanger()) {
		double best_distance = DBL_MAX, tmp_dis;
		for (tmp = grays2.begin(); tmp != grays2.end(); tmp++) {
			tmp_dis = DistanceForPackman((*tmp)->get_x(), (*tmp)->get_y());

			if (tmp_dis < best_distance) {
				best_distance = tmp_dis;
				pc = *tmp;
			}
		}
	}
	else {
		double best_distance = 0, tmp_dis;
		for (tmp = grays2.begin(); tmp != grays2.end(); tmp++) {
			tmp_dis = DistanceMaze((*tmp)->get_x(), (*tmp)->get_y(), dangerousMonster->get_x(), dangerousMonster->get_y());

			if (tmp_dis > best_distance) {
				best_distance = tmp_dis;
				pc = *tmp;
			}
		}
	}

	// Start restoring the path from the selected cell
	p = pc->get_parent();
	while (p != nullptr) {
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


void RoomAstar(Room r1, Room r2) {
	// Initialize containers for cells being explored and explored cells
	vector<Cell> grays;
	vector<Cell> blacks;

	// Initialize variables for target room coordinates
	int xTarget = r2.get_cx();
	int yTarget = r2.get_cy();

	// Create the initial cell representing the starting room
	Cell first(r1.get_cx(), r1.get_cy(), nullptr, xTarget, yTarget, 0);

	// Push the initial cell into the priority queue and mark it as gray
	pq.push(first);
	grays.push_back(first);

	// Iterator for gray cells
	vector<Cell>::iterator it_gray;

	// Flag to track whether A* algorithm is running
	bool AStar_is_running = true;

	// Main A* loop
	while (AStar_is_running) {
		// Check if the priority queue is empty
		if (pq.empty()) {
			// If the queue is empty, terminate the algorithm
			AStar_is_running = false;
			return;
		}
		else {
			// Retrieve the current cell from the priority queue
			Cell* pcurrent = new Cell(pq.top());
			pq.pop();

			// Find the current cell in the gray list
			it_gray = find(grays.begin(), grays.end(), *pcurrent);

			// Check if the current cell is not in the gray list
			if (it_gray == grays.end()) {
				// If not found in the gray list, print an error message and terminate the algorithm
				printf("Current cell not found in gray list.\n");
				AStar_is_running = false;
				return;
			}

			// Remove the current cell from the gray list and add it to the black list
			grays.erase(it_gray);
			blacks.push_back(*pcurrent);

			// Get the coordinates of the current cell
			int r = pcurrent->get_x();
			int c = pcurrent->get_y();

			// Explore neighboring cells

			// Up
			if (AStar_is_running && r < MSZ) {
				RoomCheckNeighbor(r + 1, c, pcurrent, xTarget, yTarget, grays, blacks);
			}

			// Down
			if (AStar_is_running && r > 0) {
				RoomCheckNeighbor(r - 1, c, pcurrent, xTarget, yTarget, grays, blacks);
			}

			// Left
			if (AStar_is_running && c > 0) {
				RoomCheckNeighbor(r, c - 1, pcurrent, xTarget, yTarget, grays, blacks);
			}

			// Right
			if (AStar_is_running && c < MSZ) {
				RoomCheckNeighbor(r, c + 1, pcurrent, xTarget, yTarget, grays, blacks);
			}
		}
	}
}



void RoomCheckNeighbor(int row, int col, Cell* p, int xt, int yt, vector <Cell>& grays, vector <Cell>& blacks) {
	// Define the cost of moving to the neighboring cell
	double cost;

	// Check if the neighboring cell is open space or an obstacle
	if (maze[row][col] == SPACE) {
		// If it's open space, set the cost to 1
		cost = 1.0;
	}
	else {
		// If it's an obstacle, set the cost to a higher value
		cost = ROOM_ASTAR_COST;
	}

	// Check if the neighboring cell is the target cell
	if (xt == row && yt == col) {
		// If it is, terminate the A* algorithm and draw the path
		AStar_is_running = false;
		while (!pq.empty()) {
			pq.pop();
		}
		drawPath(p);
		return;
	}
	else {
		// If it's not the target cell, proceed with evaluating the cell
		Cell* ctmp = new Cell(row, col, p, xt, yt, p->get_g_val() + cost);
		vector<Cell>::iterator it_gray = find(grays.begin(), grays.end(), *ctmp);
		vector<Cell>::iterator it_black = find(blacks.begin(), blacks.end(), *ctmp);

		// Check if the neighboring cell is unexplored (not in grays or blacks)
		if (it_gray == grays.end() && it_black == blacks.end()) {
			// If it's unexplored, add it to the priority queue and mark it as gray
			pq.push(*ctmp);
			grays.push_back(*ctmp);
		}
		else if (it_gray != grays.end()) {
			// If it's gray, check if the new path to it is better
			if (ctmp->get_g_val() < it_gray->get_g_val()) {
				// If the new path is better, update its cost and adjust the priority queue
				*it_gray = *ctmp;
				updatePriorityQueue(*ctmp);
			}
		}
	}
}

void updatePriorityQueue(Cell& cell) {
	// Temporary vector to hold cells from the priority queue
	vector<Cell> tmp;

	// Pop cells from the priority queue until the cell to be updated is found
	while (!pq.empty()) {
		Cell top_cell = pq.top();
		pq.pop();
		if (top_cell == cell) {
			// Found the cell to be updated, break the loop
			break;
		}
		tmp.push_back(top_cell);
	}

	// Update the priority queue with the updated cell
	pq.push(cell);

	// Re-insert cells from the temporary vector back into the priority queue
	for (const auto& tmp_cell : tmp) {
		pq.push(tmp_cell);
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
				glColor3d(0.35, 0.35, 0.35);// LIGHT GREY 
				break;
			case SPACE:
				glColor3d(1, 1, 1); // WHITE
				break;
			case PACMAN:
				glColor3d(0.7, 0.6, 0.85); // LIGHT PURPLE
				break;
			case MONSTER:
				glColor3d(0, 0, 0); // BLACK
				break;
			case COIN:
				glColor3d(0.8, 0.5, 0.2); // YELLOW
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
	
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE); // double means it uses double buffer
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(600, 100);
	glutCreateWindow("Pacman");

	glutDisplayFunc(display); // refresh function
	glutIdleFunc(idle); // runs all the time in the background

	glutCreateMenu(menu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutAddMenuEntry("Start Game", 1);

	init();

	glutMainLoop();
}