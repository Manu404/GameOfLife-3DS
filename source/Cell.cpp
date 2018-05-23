#include <3ds.h>
#include <stdio.h>
#include <string.h>

#include "Cell.h"

Cell::Cell(Vector2* position, int isAlive, Vector2* worldSize)
{
    this->IsAlive = isAlive;
    this->Position = position;
    this->newState = IsAlive;
    this->worldSize = worldSize;
    memset(neighbours, 0, (8 * sizeof(Cell*)));
}

void Cell::AddNeighbourgs(Cell* cell)
{
    neighbours[currentNeighbors] = cell;
    currentNeighbors += 1;
}

void Cell::ComputeState()
{
    //this->WasAlive = IsAlive;
    newState = IsAlive;

    // Compute alive cells
    int aliveCellCount = 0;
    if (currentNeighbors == 0) return;
    for (int x = 0; x < currentNeighbors; x++)
        if (neighbours[x]->IsAlive)
            aliveCellCount += 1;

    // Apply Conways lay
    if (aliveCellCount < 2 && this->IsAlive) newState = 0;
    else if (aliveCellCount <= 3 && this->IsAlive) newState = 1;
    else if (aliveCellCount > 3 && this->IsAlive) newState = 0;
    else if (aliveCellCount == 3 && !this->IsAlive) newState = 1;
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