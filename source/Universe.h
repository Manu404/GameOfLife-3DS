#ifndef UNIVERSE_H
#define UNIVERSE_H

#include "Vector2.h"
#include "Cell.h"
#include "Globals.h"

class Universe
{
    Vector2* universSize;
    Cell* cells[UNIVERSE_WIDTH][UNIVERSE_HEIGHT];

    Vector2* AdjustCoordonates(Vector2* vector);
    
    void GenerateCells();
    void PopulateNeighbourgs();
    
    void PrintCell(int x, int y, Color* color);
    void PrintPixel(int x, int y, Color* color);
    void PrintCellToOPCBuffer(int x, int y, Color* color);

    int AdjustViewport(int zoomFactor, int screenSize, int viewPort);
    int ConvertCoordonatesToMemoryLocation(int x, int y);
    int ConvertImageCoordonatesToMemoryLocation(int x, int y);
    int ConvertCoordonatesToOPCLocation(int x, int y);
public:

    int RandomFactor = 7;

    u8 universe_framebuffer[PIXELBUFFER_SIZE * 3];
    u8 universe_framebuffer_opc[UNIVERSE_WIDTH * UNIVERSE_HEIGHT * 3];
    u32 universe_framebuffer_size = PIXELBUFFER_SIZE * 3;
    u32 universe_framebuffer_size_opc = UNIVERSE_WIDTH * UNIVERSE_HEIGHT * 3;

    Universe(Vector2* size);

    void Reset();
    void Compute();
    void Print(Color* Foreground, Color* Background, int zoomFactor, int viewPortX, int viewPortY);
};

#endif