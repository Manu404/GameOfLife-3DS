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
    
    void PrintPixel(int memOffset, Color* color);

    int ConvertCoordonatesToMemoryLocation(int x, int y);

public:

    int RandomFactor = 7;

    u8 universe_framebuffer[PIXELBUFFER_SIZE * 3]; // BGR BGR BGR !!! REVERSED !!!
    u32 world_framebuffer_size = PIXELBUFFER_SIZE * 3;

    Universe(Vector2* size);

    void Reset();
    void Compute();
    void Print(Color* Foreground, Color* Background, int zoomFactor, int viewPortX, int viewPortY);
};

#endif