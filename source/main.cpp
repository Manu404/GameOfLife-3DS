// MIT License
// 
// Copyright(c) 2018 Manu404 - istace.emmanuel@hotmail.com
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <math.h>
#include <unistd.h>
#include <logo_bgr.h>

#define WIDTH 400
#define HEIGTH 240
#define PIXELBUFFER_SIZE WIDTH * HEIGTH // 96000

u8 universe_framebuffer[PIXELBUFFER_SIZE * 3]; // RGB RGB RGB RGB ... so times 3
u32 world_framebuffer_size = PIXELBUFFER_SIZE * 3; 


class Vector2 // 2D Vector
{
public:
    int X;
    int Y;

    Vector2(int x, int y)
    {
        this->X = x;
        this->Y = y;
    }
};

class Cell // Conway cell
{
    Vector2* worldSize;
    Cell* neighbours[8];
    int newState; 
    int currentNeighbors = 0;
public:
    int IsAlive;
    Vector2* Position;

    Cell(Vector2* position, int isAlive, Vector2* worldSize)
    {
        this->IsAlive = isAlive;
        this->Position = position;
        this->newState = IsAlive;
        this->worldSize = worldSize;
        memset(neighbours, 0, (8 * sizeof(Cell*)));
    }

    void AddNeighbourgs(Cell* cell)
    {
        neighbours[currentNeighbors] = cell;
        currentNeighbors += 1;
    }

    void ComputeState()
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

    Vector2* AdjustCoordonates(Vector2* vector)
    {
        if (vector->X >= worldSize->X) vector->X = 0;
        else if (vector->X < 0) vector->X = worldSize->X - 1;
        if (vector->Y >= worldSize->Y) vector->Y = 0;
        else if (vector->Y < 0) vector->Y = worldSize->Y - 1;
        return vector;
    }

    void SetNewstate(int state)
    {
        this->IsAlive = state;
        this->newState = state;
    }

    void ApplyNewState()
    {
        this->IsAlive = this->newState;
    }
};

class Universe
{
    Vector2* universSize;
    int randFactor = 7;
public:

    size_t rows;
    size_t cols;
    Cell* cells[WIDTH][HEIGTH];

    Universe(Vector2* size)
    {
        this->universSize = size;
        memset(cells, 0, (size->X * size->Y * sizeof(Cell*)));
    }

    void GenerateCells()
    {
        // Initialize random cells data
        for (int x = 0; x < universSize->X; x++)
            for (int y = 0; y < universSize->Y; y++)
                cells[x][y] = new Cell(new Vector2(x, y), rand() % randFactor == 0, this->universSize);
    }

    void PopulateNeighbourgs()
    {
        for (int x = 0; x < universSize->X; x++)
            for (int y = 0; y < universSize->Y; y++)
            {
                for (int x2 = -1; x2 <= 1; x2++) // Iterate around the current position
                    for(int y2 = -1; y2 <= 1; y2++)
                    {
                        Vector2* neighbourPosition = AdjustCoordonates(new Vector2(x + x2, y + y2));

                        if (neighbourPosition->X == x && neighbourPosition->Y == y) continue;

                        cells[x][y]->AddNeighbourgs(cells[neighbourPosition->X][neighbourPosition->Y]);
                    }
            }
    }

    void Reset()
    {
        for (int x = 0; x < universSize->X; x++)
            for (int y = 0; y < universSize->Y; y++) {
                cells[x][y]->SetNewstate((rand() % randFactor) == 0);
            }
    }

    Vector2* AdjustCoordonates(Vector2* vector)
    {
        if (vector->X >= this->universSize->X) vector->X = 0;
        else if (vector->X < 0) vector->X = this->universSize->Y - 1;
        if (vector->Y >= this->universSize->Y) vector->Y = 0;
        else if (vector->Y < 0) vector->Y = this->universSize->Y - 1;
        return vector;
    }

    void Compute()
    {
        for (int x = 0; x < universSize->X; x++)
            for (int y = 0; y < universSize->Y; y++)
            {
                cells[x][y]->ComputeState();
            }
    }

    void Print()
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
};

int main(int argc, char **argv)
{
	gfxInitDefault();

	//Initialize console on top screen. Using NULL as the second argument tells the console library to use the internal console structure as current one
	//consoleInit(GFX_BOTTOM, NULL);
    
	//We don't need double buffering in this example. In this way we can draw our image only once on screen.
    gfxSetDoubleBuffering(GFX_TOP, false);
    gfxSetDoubleBuffering(GFX_BOTTOM, false);

    //Get the bottom screen's frame buffer
    u8* top_framebuffer = gfxGetFramebuffer(GFX_TOP, GFX_RIGHT, NULL, NULL);

    u8* bottom_framebuffer = gfxGetFramebuffer(GFX_BOTTOM, GFX_RIGHT, NULL, NULL);
    memcpy(bottom_framebuffer, logo_bgr, logo_bgr_size);

    Universe* universe = new Universe(new Vector2(WIDTH, HEIGTH));
    universe->GenerateCells();
    universe->PopulateNeighbourgs();
    universe->Compute();

    int frameCount = 0;
    int speed = 1;


    u32 kDownOld = 0, kHeldOld = 0, kUpOld = 0; //In these variables there will be information about keys detected in the previous frame

	// Main loop
    while (aptMainLoop())
    {
        frameCount += 1;
        universe->Print();

        if (frameCount % speed == 0)
            universe->Compute();

        //Copy our image in the bottom screen's frame buffer
        memcpy(top_framebuffer, universe_framebuffer, world_framebuffer_size);

        //Scan all the inputs. This should be done once for each frame
        hidScanInput();

        //hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
        u32 kDown = hidKeysDown();
        //hidKeysHeld returns information about which buttons have are held down in this frame
        u32 kHeld = hidKeysHeld();
        //hidKeysUp returns information about which buttons have been just released
        u32 kUp = hidKeysUp();

        if (kDown & KEY_START) break; // break in order to return to hbmenu

        if (kDown != kDownOld || kHeld != kHeldOld || kUp != kUpOld)
        {
            if (kDown & KEY_SELECT)
                universe->Reset();

            if (kDown & KEY_DUP)
            {
                if (speed > 0)
                    speed -= 1;
            }

            if (kDown & KEY_DDOWN)
                speed += 1;

        }
		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();

		//Wait for VBlank
		gspWaitForVBlank();
	}

	// Exit services
	gfxExit();
	return 0;
}
