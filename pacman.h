#ifndef Pacman_header
#define Pacman_header
#include "move.h"

class Pacman : public Move {
public:
    Pacman(int _x, int _y);
    void movePac(int dirx, int diry);
    int nextDirX, nextDirY;
};

#endif