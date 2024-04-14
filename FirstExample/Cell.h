#pragma once
class Cell
{
private:
	Cell* parent;
	int x, y;
	double h_val, f_val, g_val;
	int tx, ty;
public:
	Cell();
	Cell(const Cell& other);
	Cell(int x, int y, Cell* p);
	Cell(int x, int y, Cell* p, int tx, int ty, double g_val);
	Cell(int x, int y, Cell* p, double v, int d);
	bool operator == (const Cell& other);
	Cell* get_parent() { return parent; }
	int get_x() { return x; }
	int get_y() { return y; }
	double get_h_val() { return h_val; }
	double get_f_val() { return g_val + h_val; }
	double get_g_val() { return g_val; }
	void set_g_val(double value) { g_val = value; }
	void set_x_val(int value) { x= value; }
	void set_y_val(int value) { y = value; }
	void h_func();
	void f_func();
};

