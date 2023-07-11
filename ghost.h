#ifndef Ghost_header
#define Ghost_header
#include "move.h"

class Ghost : public Move {
private:
    int currentDir;
    int lastVal;
    bool turn = false;
public:
    Ghost(int _x, int _y);
    bool checkForPacman();
    void move(float time);
    int newDirection();
};

#endif