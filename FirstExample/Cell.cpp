#include "Cell.h"
#include <math.h>


Cell::Cell()
{
}

Cell::Cell(int x, int y, Cell* p)
{
	this->x = x;
	this->y = y;
	parent = p;
	ty = x;
	tx = y;
	g_val = 0;
	h_func();
	f_func();

}

Cell::Cell(int x, int y, Cell * p, int tx, int ty, double g)
{
	this->x = x;
	this->y = y;
	parent = p;
	this->ty = tx;
	this->tx = ty;
	this->g_val = g;
	h_func();
	f_func();

}

Cell::Cell(int x, int y, Cell* p, double v, int d)
{
	this->x = x;
	this->y = y;
	ty = 0;
	tx = 0;
	parent = p;
	h_val = v;
	g_val = d;
	f_val = g_val + h_val;
}


Cell::Cell(const Cell &other) {
	x = other.x;
	y = other.y;
	parent = other.parent;
	ty = other.ty;
	tx = other.tx;
	g_val = other.g_val;
	h_func();
	f_func();
}

bool Cell::operator==(const Cell& other)
{
	return (x == other.x) && (y == other.y);
}


void Cell::h_func()
{
	h_val = sqrt(pow((double)x - (double)tx, 2) + pow((double)y - (double)ty, 2));
}

void Cell::f_func()
{
	f_val = g_val + h_val;
}

