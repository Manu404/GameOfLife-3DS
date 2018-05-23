#include "Universe.h"
#include "Cell.h"
#include <string.h>
#include <stdlib.h>
#include "Globals.h"

Universe::Universe(Vector2* size)
{
    this->universSize = size;
    memset(cells, 0, (size->X * size->Y * sizeof(Cell*)));
    GenerateCells();
    PopulateNeighbourgs();
}

void Universe::GenerateCells()
{
    // Initialize random cells data
    for (int x = 0; x < universSize->X; x++)
        for (int y = 0; y < universSize->Y; y++)
            cells[x][y] = new Cell(new Vector2(x, y), rand() % RandomFactor == 0, this->universSize);
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
            cells[x][y]->SetNewstate((rand() % RandomFactor) == 0);
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

void Universe::Print(Color* Foreground, Color* Background)
{
    for (int x = 0; x < universSize->X; x++) {
        int xMemOffset = (x * this->universSize->Y);
        for (int y = 0; y < universSize->Y; y++)
        {
            cells[x][y]->ApplyNewState();

            int memOffset = (xMemOffset + y) * 3;
            if (cells[x][y]->IsAlive)
            {
                universe_framebuffer[memOffset] = Foreground->B;
                universe_framebuffer[memOffset + 1] = Foreground->G;
                universe_framebuffer[memOffset + 2] = Foreground->R;
            }
            else
            {
                universe_framebuffer[memOffset] = Background->B;
                universe_framebuffer[memOffset + 1] = Background->R;
                universe_framebuffer[memOffset + 2] = Background->G;
            }
        }
    }
}

