#include"move.h"
#include<vector>;

using namespace std;
extern vector<vector<int>> level;

Move::Move(int _x, int _y) {
	x = startPosx = _x;
	y = startPosy = _y;
}

//Checks if specified coordinates are a wall or not. (basically collisions)
bool Move::checkDir(int dirx, int diry) {
	//Checks if new position is valid or not
	int _x = x + dirx;
	int _y = y + diry;

	bool outOfX = (_x < 0 || _x > level.size() - 1);
	bool outOfY = (_y < 0 || _y > level[1].size() - 1);
	bool outOfBounds = (outOfX || outOfY);

	if (outOfBounds) { return true; }
	else if (level[_x][_y] != 1) return true;
	return false;
}
