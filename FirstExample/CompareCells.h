#pragma once
#include "Cell.h"
class CompareCells
{
public:
	bool operator()(Cell c1, Cell c2) { return c1.get_f_val() > c2.get_f_val(); }
};
