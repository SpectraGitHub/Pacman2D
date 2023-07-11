#ifndef Move_header
#define Move_header

class Move {
public:
    bool transform = false;
    bool wrap = false;
    float lastTime = -1;
    float linTime = 0;
    int dirx; int diry;
    int x, y, startPosx, startPosy, pastX, pastY;
    Move(int _x, int _y);
    bool checkDir(int dirx, int diry);
};

#endif