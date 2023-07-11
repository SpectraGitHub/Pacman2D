#include"pacman.h"
#include<vector>;

using namespace std;
extern vector<vector<int>> level;

Pacman::Pacman(int _x, int _y) : Move(_x, _y) {
	nextDirX = 0;
	nextDirY = 0;
}

/// <summary>
/// Calculates and moves pacman to new location
/// </summary>
/// <param name="dirx"></param>
/// <param name="diry"></param>
void Pacman::movePac(int dirx, int diry) {
	transform = true; //Will now animate 
	level[x][y] = -1; // No pellet anymore

	//Interpolation variables
	pastX = x;
	pastY = y;

	//Checks for bounds
	bool outOfX = (x+dirx < 0 || x+dirx > level.size() - 1);
	bool outOfY = (y+diry < 0 || y+diry > level[1].size() - 1);
	if (outOfX) { // x correction
		if (x < level.size() / 2) { x = level.size() - 1; }
		else { x = 0; }
		wrap = true;
	}
	else if (outOfY) { // y ccorrection
		if (y < level[1].size() / 2) { y = level[1].size() - 1; }
		else { y = 0; }
		wrap = true;
	}
	else { //Normal moving
		x += dirx;
		y += diry;
	}

	level[x][y] = 2;
}