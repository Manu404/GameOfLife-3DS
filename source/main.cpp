#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <math.h>
#include <unistd.h>

#define WIDTH 400
#define HEIGTH 240
#define PIXELBUFFER_SIZE WIDTH * HEIGTH // 96000

u8 brew2_bgr[PIXELBUFFER_SIZE * 3]; // RGB RGB RGB RGB ... so times truie as french says
u32 brew2_bgr_size = PIXELBUFFER_SIZE * 3; 

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
    Cell* neighbours[8];
    int newState; 
    int currentNeighbors = 0;
public:
    int IsAlive;
    Vector2* Position;

    Cell(Vector2* position, int isAlive)
    {
        this->IsAlive = isAlive;
        this->Position = position;
    }

    void AddNeighbourgs(Cell* cell)
    {
        neighbours[currentNeighbors] = cell;
        currentNeighbors += 1;
    }

    void ComputeState()
    {
        newState = IsAlive;

        // Compute alive cells
        int aliveCellCount = 0;
        for (int x = 0; x < 8; x++)
            if (neighbours[x]->IsAlive)
                aliveCellCount += 1;

        // Apply Conways lay
        if (aliveCellCount < 2 && this->IsAlive) newState = 0;
        else if (aliveCellCount <= 3 && this->IsAlive) newState = 1;
        else if (aliveCellCount > 3 && this->IsAlive) newState = 0;
        else if (aliveCellCount == 3 && !this->IsAlive) newState = 1;
    }

    void ApplyNewState()
    {
        this->IsAlive = this->newState;
    }
};

class World
{
    Vector2* size;
    Thread computeThread[WIDTH];
    Cell* cells[WIDTH][HEIGTH];
public:
    World(Vector2* size)
    {
        this->size = size;
        memset(cells, 0, size->X * size->Y);
    }

    void GenerateCells()
    {
        // Initialize random cells data
        for (int x = 0; x < size->X; x++)
            for (int y = 0; y < size->Y; y++)
                cells[x][y] = (new Cell(new Vector2(x, y), rand() % 7 == 0));
    }

    void PopulateNeighbourgs()
    {
        for (int x = 0; x < size->X; x++)
            for (int y = 0; y < size->Y; y++)
            {
                // Lazy way to direct access neighbors. If coord are out of screen they're adjusted by AdjustCooronates
                Vector2* top_left = AdjustCoordonates(new Vector2(x + 1, y - 1));
                Vector2* top_center = AdjustCoordonates(new Vector2(x + 1, y));
                Vector2* top_right = AdjustCoordonates(new Vector2(x + 1, y + 1));

                Vector2* middle_left = AdjustCoordonates(new Vector2(x, y - 1));
                Vector2* middle_right = AdjustCoordonates(new Vector2(x, y + 1));
                
                Vector2* bottom_left = AdjustCoordonates(new Vector2(x - 1, y - 1));
                Vector2* bottom_center = AdjustCoordonates(new Vector2(x - 1, y));
                Vector2* bottom_right = AdjustCoordonates(new Vector2(x - 1, y + 1));

                // Add each neighboors to neighboorhood
                cells[x][y]->AddNeighbourgs(cells[top_left->X][top_left->Y]);
                cells[x][y]->AddNeighbourgs(cells[top_center->X][top_center->Y]);
                cells[x][y]->AddNeighbourgs(cells[top_right->X][top_right->Y]);

                cells[x][y]->AddNeighbourgs(cells[middle_left->X][middle_left->Y]);
                cells[x][y]->AddNeighbourgs(cells[middle_right->X][middle_right->Y]);

                cells[x][y]->AddNeighbourgs(cells[bottom_left->X][bottom_left->Y]);
                cells[x][y]->AddNeighbourgs(cells[bottom_center->X][bottom_center->Y]);
                cells[x][y]->AddNeighbourgs(cells[bottom_right->X][bottom_right->Y]);
            }
    }

    Vector2* AdjustCoordonates(Vector2* vector)
    {
        if (vector->X > this->size->Y) vector->X = 0;
        else if (vector->X < 0) vector->X = this->size->Y;
        if (vector->Y > this->size->Y) vector->Y = 0;
        else if (vector->Y < 0) vector->Y = this->size->X;
        return vector;
    }

    void Compute()
    {

        for (int x = 0; x < size->X; x++)
            for (int y = 0; y < size->Y; y++)
            {
                cells[x][y]->ComputeState();
            }

        for (int x = 0; x < size->X; x++)
            for (int y = 0; y < size->Y; y++)
            {
                cells[x][y]->ApplyNewState();
            }
    }

    void Print()
    {
        for (int x = 0; x < size->Y; x++)
            for (int y = 0; y < size->X; y++)
            {
                int color = cells[x][y]->IsAlive ? 255 : 0;

                // RGB so times three
                brew2_bgr[(x * this->size->X * 3) + y * 3] = color;// ? rand() % 255 : 0;
                brew2_bgr[(x * this->size->X * 3) + y * 3 + 1] = color;// ? rand() % 255 : 0;
                brew2_bgr[(x * this->size->X * 3) + y * 3 + 2] = color;// ? rand() % 255 : 0;
            }
    }
};

int main(int argc, char **argv)
{
	gfxInitDefault();

	//Initialize console on top screen. Using NULL as the second argument tells the console library to use the internal console structure as current one
	consoleInit(GFX_BOTTOM, NULL);

    printf("For hackers... with love.\n");
    printf("https://lghs.be\n");
    
	//We don't need double buffering in this example. In this way we can draw our image only once on screen.
	gfxSetDoubleBuffering(GFX_TOP, false);

    //Get the bottom screen's frame buffer
    u8* fb = gfxGetFramebuffer(GFX_TOP, GFX_RIGHT, NULL, NULL);

    World* w = new World(new Vector2(WIDTH, HEIGTH));
    w->GenerateCells();
    w->PopulateNeighbourgs();

    int frameCount = 0;
	// Main loop
	while (aptMainLoop())
	{
        frameCount += 1;
        //if(frameCount % 60 == 0)
        {
            w->Compute();            
        }

        w->Print();

        //Copy our image in the bottom screen's frame buffer
        memcpy(fb, brew2_bgr, brew2_bgr_size);

		//Scan all the inputs. This should be done once for each frame
		hidScanInput();

		//hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
		u32 kDown = hidKeysDown();

		if (kDown & KEY_START) break; // break in order to return to hbmenu

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
