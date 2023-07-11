#include"ghost.h";
#include<vector>;
#include <iostream>;

using namespace std;
extern vector<vector<int>> level;
extern bool gameOver;

Ghost::Ghost(int _x, int _y) : Move(_x, _y)
{
	level[x][y] = 3;
	lastVal = dirx = diry = 0;

	//generate start direction
	currentDir = newDirection();
}

/// <summary>
/// Finds an initial direction to travel after spawning
/// </summary>
/// <returns>new direction</returns>
int Ghost::newDirection() {
	if (checkDir(1, 0)) { return 0; }
	else if (checkDir(-1, 0)) { return 1; }
	else if (checkDir(0, 1)) { return 2; }
	else if (checkDir(0, -1)) { return 3; }
}


/// <summary>
/// Ghost AI with three stages, Discovery, Choice & Action
/// discovers possible moves,
/// chooses one of all available
/// performs that choice by moving to that location
/// </summary>
/// <param name="time">Update the ghosts own time keeping</param>
void Ghost::move(float time) {
	transform = true;

	int options[4] = {0,0,0,0};
	int tempx, tempy;

	//DISCOVERY
	int j = 0;
	for (int i = 0; i < 4; i++) {
		switch (i)
		{
		case 0: tempx =  1; tempy =  0;  break;
		case 1:	tempx = -1; tempy =  0;  break;
		case 2: tempx =  0; tempy =  1;  break;
		case 3: tempx =  0; tempy = -1;  break;
		default: cerr << "Something went wrong with ghost ai"; return;
		}
		if (checkDir(tempx, tempy) && ((tempx != -dirx && tempy == 0) || ( tempy != -diry && tempx == 0))) {
			options[j] = i;
			j++;
		}
	}

	//CHOICE
	int choice = 0;
	switch (j) {
	case 0: turn = true; break;
	case 1: currentDir = options[j-1];	break;
	case 2: 		
		choice = rand() % 2;
		currentDir = options[choice];	break;
	case 3: 
		choice = rand() % 3;
		currentDir = options[choice];	break;
	}
	
	//ACTION
	switch (currentDir)
	{
	case 0: dirx = 1; diry = 0; break; //cout << "East"  << endl; break;
	case 1:	dirx = -1; diry = 0; break; //cout << "West"  << endl; break;
	case 2: dirx = 0; diry = 1; break; //cout << "South" << endl; break;
	case 3: dirx = 0; diry = -1; break; //cout << "North" << endl; break;
	default: cerr << "Something went wrong "; return;
	}

	//If no way other than backwards, flip the direction
	if (turn) {
		if (dirx == 0) {
			diry *= -1;
		}
		else if(diry == 0) {
			dirx *= -1;
		}
		turn = false;
	}

	//Check pacMan at previous position
	checkForPacman();

	// reset previous value on position
	level[x][y] = lastVal;
	pastX = x;
	pastY = y;


	//Checks for bounds
	bool outOfX = (x + dirx < 0 || x + dirx > level.size() - 1);
	bool outOfY = (y + diry < 0 || y + diry > level[1].size() - 1);
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

	lastVal = level[x][y];

	//Check for pacMan at new position
	if (!checkForPacman()) { // print own position
		level[x][y] = 3;
	}
	lastTime = time;
}

//Checks if pacman is at ghosts location and ends the game if he is
bool Ghost::checkForPacman() {
	//Check for pacMan
	if (level[x][y] == 2) {
		gameOver = true;
		cout << endl << endl << "game Over" << endl << endl;
		return true;
	}
		return false;;
}