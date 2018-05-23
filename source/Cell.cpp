#include <3ds.h>
#include <stdio.h>
#include <string.h>

#include "Cell.h"
#include <stdlib.h>

Cell::Cell(Vector2* position, int isAlive, Vector2* worldSize, Color* color)
{
    this->IsAlive = isAlive;
    this->Position = position;
    this->newState = IsAlive;
    this->worldSize = worldSize;
    this->color = color;
    memset(neighbours, 0, (8 * sizeof(Cell*)));
}

void Cell::AddNeighbourgs(Cell* cell)
{
    neighbours[currentNeighbors] = cell;
    currentNeighbors += 1;
}

void Cell::ComputeState()
{
    // Compute alive cells
    int aliveCellCount = 0;
    for (int x = 0; x < currentNeighbors; x++)
        if (neighbours[x]->IsAlive)
            aliveCellCount += 1;

    // Apply Conways lay
    if(this->IsAlive)
    {
        if (aliveCellCount < 2) newState = 0;
        else if (aliveCellCount <= 3) newState = 1;
        else newState = 0;
    }
    else if (aliveCellCount == 3) newState = 1;
}

Vector2* Cell::AdjustCoordonates(Vector2* vector)
{
    if (vector->X >= worldSize->X) vector->X = 0;
    else if (vector->X < 0) vector->X = worldSize->X - 1;
    if (vector->Y >= worldSize->Y) vector->Y = 0;
    else if (vector->Y < 0) vector->Y = worldSize->Y - 1;
    return vector;
}

void Cell::SetNewstate(int state)
{
    this->IsAlive = state;
    this->newState = state;
}

void Cell::ApplyNewState()
{
    this->IsAlive = this->newState;
}