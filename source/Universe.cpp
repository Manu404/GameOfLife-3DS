#include "Universe.h"
#include "Cell.h"
#include <string.h>
#include <stdlib.h>
#include "Globals.h"

Universe::Universe(Vector2* size)
{
    this->universSize = size;
    memset(cells, 0, (size->X * size->Y * sizeof(Cell*)));
}

void Universe::GenerateCells()
{
    // Initialize random cells data
    for (int x = 0; x < universSize->X; x++)
        for (int y = 0; y < universSize->Y; y++)
            cells[x][y] = new Cell(new Vector2(x, y), rand() % randFactor == 0, this->universSize);
}

void Universe::PopulateNeighbourgs()
{
    for (int x = 0; x < universSize->X; x++)
        for (int y = 0; y < universSize->Y; y++)
        {
            for (int x2 = -1; x2 <= 1; x2++) // Iterate around the current position
                for (int y2 = -1; y2 <= 1; y2++)
                {
                    Vector2* neighbourPosition = AdjustCoordonates(new Vector2(x + x2, y + y2));

                    if (neighbourPosition->X == x && neighbourPosition->Y == y) continue;

                    cells[x][y]->AddNeighbourgs(cells[neighbourPosition->X][neighbourPosition->Y]);
                }
        }
}

void Universe::Reset()
{
    for (int x = 0; x < universSize->X; x++)
        for (int y = 0; y < universSize->Y; y++) {
            cells[x][y]->SetNewstate((rand() % randFactor) == 0);
        }
}

Vector2* Universe::AdjustCoordonates(Vector2* vector)
{
    if (vector->X >= this->universSize->X) vector->X = 0;
    else if (vector->X < 0) vector->X = this->universSize->Y - 1;
    if (vector->Y >= this->universSize->Y) vector->Y = 0;
    else if (vector->Y < 0) vector->Y = this->universSize->Y - 1;
    return vector;
}

void Universe::Compute()
{
    for (int x = 0; x < universSize->X; x++)
        for (int y = 0; y < universSize->Y; y++)
        {
            cells[x][y]->ComputeState();
        }
}

void Universe::Print()
{
    for (int x = 0; x < universSize->X; x++)
        for (int y = 0; y < universSize->Y; y++)
        {
            cells[x][y]->ApplyNewState();
            int color = cells[x][y]->IsAlive ? 255 : 0;

            // RGB so times three
            int memOffset = ((x * this->universSize->Y) + (y)) * 3;
            universe_framebuffer[memOffset] = color;// ? rand() % 255 : 0;
            universe_framebuffer[memOffset + 1] = color;// ? rand() % 255 : 0;
            universe_framebuffer[memOffset + 2] = color;// ? rand() % 255 : 0;*/
        }
}