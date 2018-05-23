#ifndef UNIVERSE_H
#define UNIVERSE_H

#include "Vector2.h"
#include "Cell.h"
#include "Globals.h"

class Universe
{
    Vector2* universSize;
    Cell* cells[WIDTH][HEIGTH];

    Vector2* AdjustCoordonates(Vector2* vector);
    
    void GenerateCells();
    void PopulateNeighbourgs();

public:

    int RandomFactor = 7;
    Color *Foreground, *Background;

    u8 universe_framebuffer[PIXELBUFFER_SIZE * 3]; // BGR BGR BGR !!! REVERSED !!!
    u32 world_framebuffer_size = PIXELBUFFER_SIZE * 3;

    Universe(Vector2* size);

    void Reset();
    void Compute();
    void Print(Color* Foreground, Color* Background);
};

#endif