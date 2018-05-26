#include "Universe.h"
#include "Cell.h"
#include <string.h>
#include <stdlib.h>
#include "Globals.h"
#include <stdio.h>

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
                    if (x2 == 0 && y2 == 0) continue;

                    Vector2* neighbourPosition = AdjustCoordonates(new Vector2(x + x2, y + y2));

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
    else if (vector->X < 0) vector->X = this->universSize->X - 1;
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

void Universe::PrintPixel(int x, int y, Color* color)
{
    const int memOffset = ConvertCoordonatesToMemoryLocation(x, y);

    if (memOffset >= world_framebuffer_size) return; // just to be sure

    universe_framebuffer[memOffset] = color->B;
    universe_framebuffer[memOffset + 1] = color->G;
    universe_framebuffer[memOffset + 2] = color->R;
}

int Universe::ConvertCoordonatesToMemoryLocation(int x, int y)
{
    return ((x * this->universSize->Y) + y) * 3;
}

int AdjustViewport(int zoomFactor, int screenSize, int viewPort)
{
    if (viewPort + (screenSize / zoomFactor) > screenSize - 1)
        return screenSize - (screenSize / zoomFactor);
    return viewPort;
}

// This method would need a lot of refactorings, mainly extract methods, but until then I'll write comments, sorry.
void Universe::Print(Color* Foreground, Color* Background, int zoomFactor, int viewPortX, int viewPortY)
{
    // The current displayed cell position
    int currentX = 0;
    int currentY = 0;

    // Adjust viewport to avoid out of bound
    viewPortX = AdjustViewport(zoomFactor, WIDTH, viewPortX);
    viewPortY = AdjustViewport(zoomFactor, HEIGTH, viewPortY);
    
    // compute viewport min max coord
    const int minX = viewPortX;
    const int minY = viewPortY;
    const int maxX = viewPortX + ((universSize->X - 1) / (zoomFactor));
    const int maxY = viewPortY + ((universSize->Y - 1) / (zoomFactor));

    // iterate over each cell
    for (int x = 0; x < universSize->X; x++) {
        for (int y = 0; y < universSize->Y; y++)
        {
            cells[x][y]->ApplyNewState();

            // if cell is outside the viewport, continue
            if (x > (maxX) || y > (maxY) || x < (minX) || y < (minY)) continue;

            // iterate over zoomfactor, x axis, if zoom is 2 we need to draw two pixel instead of one
            for (int xi = 0; xi < zoomFactor; xi++) {

                // if out of bound, continue
                if ((currentX * zoomFactor) + xi > WIDTH) continue;

                // iterate over zoomfactor, y axis
                for (int yi = 0; yi < zoomFactor; yi++) {

                    // if out of bound, continue
                    if ((currentY * zoomFactor) + yi > HEIGTH) continue;

                    // Compute pixel position and get memory location
                    const int pixelToDraw_x = (currentX * zoomFactor) + xi;
                    const int pixelToDraw_y = (currentY * zoomFactor) + yi;

                    // Draw border
                    if (x + xi == 0 || (x + xi == WIDTH - 1 && xi == 0)
                        || y + yi == 0 || (y + yi == HEIGTH - 1 && yi == 0))
                    {
                        PrintPixel(pixelToDraw_x, pixelToDraw_y, new Color(255, 255, 255));
                    }
                    else
                    {
                        // Draw correct color to memory location
                        PrintPixel(pixelToDraw_x, pixelToDraw_y, cells[x][y]->IsAlive ? Foreground : Background);
                    }

                }
            }

            // if we're here, we've drawn a cell, go to next one
            if (currentY == ((HEIGTH - 1) / zoomFactor))
            {
                currentX += 1;
                currentY = 0;
            }
            else {
                currentY += 1;
            }
        }
    }
}

