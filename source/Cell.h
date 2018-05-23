#ifndef CELL_H
#define CELL_H

#include "Vector2.h"

class Cell // Conway cell
{
    Vector2* worldSize;
    Cell* neighbours[8];
    int newState;
    int currentNeighbors = 0;
public:
    int IsAlive;
    Vector2* Position;

    Cell(Vector2* position, int isAlive, Vector2* worldSize);
    void AddNeighbourgs(Cell* cell);
    void ComputeState();
    Vector2* AdjustCoordonates(Vector2* vector);
    void SetNewstate(int state);
    void ApplyNewState();
};
#endif