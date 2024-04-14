#pragma once

#include "Constants.h"
class Room
{
private:
	int centerX;
	int centerY;
	int width;
	int hieght;
public:
	Room() {

		centerY = 0;
		centerX = 0;
		hieght = 0;
		width = 0;
	};
	Room(int x, int y, int h, int w) {

		set_cx(x);
		set_cy(y);
		set_w(w);
		set_h(h);
	};
	void set_cx(int r) { centerX = r; }
	void set_cy(int c) { centerY = c; }
	void set_w(int w) { width = w; }
	void set_h(int h) { hieght = h; }
	int get_cx() { return centerX; }
	int get_cy() { return centerY; }
	int get_w() { return width; }
	int get_h() { return hieght; }
	void fillRoom(int maze[MSZ][MSZ], int value) {
		int i, j;
		for (i = centerX - hieght / 2; i <= centerX + hieght / 2; i++)
			for (j = centerY - width / 2; j <= centerY + width / 2; j++)
				maze[i][j] = value;
	};
};

