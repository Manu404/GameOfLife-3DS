#ifndef UNIVERSE_H
#define UNIVERSE_H

#include "Vector2.h"
#include "Cell.h"
#include "Globals.h"

class Universe
{
    Vector2* universSize;
    int randFactor = 7;
    Cell* cells[WIDTH][HEIGTH];

    Vector2* AdjustCoordonates(Vector2* vector);
    

public:

    u8 universe_framebuffer[PIXELBUFFER_SIZE * 3]; // RGB RGB RGB RGB ... so times 3
    u32 world_framebuffer_size = PIXELBUFFER_SIZE * 3;

    Universe(Vector2* size);
    void GenerateCells();
    void PopulateNeighbourgs();

    void Reset();
    void Compute();
    void Print();
};

#endif