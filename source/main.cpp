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
#define PIXELBUFFER_SIZE 96000 // 96000

u8 world_framebuffer[PIXELBUFFER_SIZE * 3]; // RGB RGB RGB RGB ... so times 3
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
    Cell* neighbours[8];
    std::list<Cell*> neighbours2;
    int newState; 
    int currentNeighbors = 0;
public:
    int IsAlive;
    Vector2* Position;

    Cell(Vector2* position, int isAlive)
    {
        this->IsAlive = isAlive;
        this->Position = position;
        newState = IsAlive;
        memset(neighbours, 0, (8 * sizeof(Cell*)));
    }

    int GetState()
    {
        return IsAlive;
    }

    void AddNeighbourgs(Cell* cell)
    {
        neighbours[currentNeighbors] = cell;
        currentNeighbors += 1;
        neighbours2.push_back(cell);
    }

    void ComputeState(Cell* cells [WIDTH][HEIGTH])
    {
        //this->WasAlive = IsAlive;
        newState = IsAlive;

        // Compute alive cells
        int aliveCellCount = 0;
        /*if (currentNeighbors == 0) return;
        for (int x = 0; x < currentNeighbors; x++)
            if (neighbours[x]->IsAlive)
                aliveCellCount += 1;*/
        std::list<Cell*>::const_iterator iterator;

        for (iterator = neighbours2.begin(); iterator != neighbours2.end(); ++iterator) {
            if ((*iterator)->GetState())
                aliveCellCount += 1;
        }

        /*Vector2* top_left = AdjustCoordonates(new Vector2(Position->X + 1, Position->Y - 1));
        Vector2* top_center = AdjustCoordonates(new Vector2(Position->X + 1, Position->Y));
        Vector2* top_right = AdjustCoordonates(new Vector2(Position->X + 1, Position->Y + 1));

        Vector2* middle_left = AdjustCoordonates(new Vector2(Position->X, Position->Y - 1));
        Vector2* middle_right = AdjustCoordonates(new Vector2(Position->X, Position->Y + 1));

        Vector2* bottom_left = AdjustCoordonates(new Vector2(Position->X - 1, Position->Y - 1));
        Vector2* bottom_center = AdjustCoordonates(new Vector2(Position->X - 1, Position->Y));
        Vector2* bottom_right = AdjustCoordonates(new Vector2(Position->X - 1, Position->Y + 1));

        if(cells[top_left->X][top_left->Y]->IsAlive)aliveCellCount+=1;
        if(cells[top_center->X][top_center->Y]->IsAlive)aliveCellCount += 1;
        if(cells[top_right->X][top_right->Y]->IsAlive)aliveCellCount += 1;
        
        if(cells[middle_left->X][middle_left->Y]->IsAlive)aliveCellCount += 1;
        if(cells[middle_right->X][middle_right->Y]->IsAlive)aliveCellCount += 1;
        
        if(cells[bottom_left->X][bottom_left->Y]->IsAlive)aliveCellCount += 1;
        if(cells[bottom_center->X][bottom_center->Y]->IsAlive)aliveCellCount += 1;
        if(cells[bottom_right->X][bottom_right->Y]->IsAlive)aliveCellCount += 1;*/
        
        // Apply Conways lay
        if (aliveCellCount < 2 && this->IsAlive) newState = 0;
        else if (aliveCellCount <= 3 && this->IsAlive) newState = 1;
        else if (aliveCellCount > 3 && this->IsAlive) newState = 0;
        else if (aliveCellCount == 3 && !this->IsAlive) newState = 1;
    }

    Vector2* AdjustCoordonates(Vector2* vector)
    {
        if (vector->X >= WIDTH) vector->X = 0;
        else if (vector->X < 0) vector->X = WIDTH - 1;
        if (vector->Y >= HEIGTH) vector->Y = 0;
        else if (vector->Y < 0) vector->Y = HEIGTH - 1;
        return vector;
    }

    void ApplyNewState()
    {
        this->IsAlive = this->newState;
    }
};


class World
{
    Vector2* size;
public:

    Cell * cells[WIDTH][HEIGTH];

    World(Vector2* size)
    {
        this->size = size;
        //memset(cells, 0, (size->X * size->Y * sizeof(Cell*)));
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
        if (vector->X >= this->size->X) vector->X = 0;
        else if (vector->X < 0) vector->X = this->size->Y - 1;
        if (vector->Y >= this->size->Y) vector->Y = 0;
        else if (vector->Y < 0) vector->Y = this->size->Y - 1;
        return vector;
    }

    void Compute()
    {
        for (int x = 0; x < size->X; x++)
            for (int y = 0; y < size->Y; y++)
            {
                cells[x][y]->ComputeState(cells);
            }
    }

    void Print()
    {
        for (int x = 0; x < size->X; x++)
            for (int y = 0; y < size->Y; y++)
            {
                cells[x][y]->ApplyNewState();
                int color = cells[x][y]->IsAlive ? 255 : 0;

                // RGB so times three
                int memOffset = ((x * this->size->Y) + (y)) * 3;
                world_framebuffer[memOffset] = color;// ? rand() % 255 : 0;
                world_framebuffer[memOffset + 1] = color;// ? rand() % 255 : 0;
                world_framebuffer[memOffset + 2] = color;// ? rand() % 255 : 0;*/
            }
    }
};

int main(int argc, char **argv)
{
	gfxInitDefault();

	//Initialize console on top screen. Using NULL as the second argument tells the console library to use the internal console structure as current one
	//consoleInit(GFX_BOTTOM, NULL);

    //printf("For hackers... with love.\n");
    //printf("https://lghs.be\n");
    
	//We don't need double buffering in this example. In this way we can draw our image only once on screen.
    gfxSetDoubleBuffering(GFX_TOP, false);
    gfxSetDoubleBuffering(GFX_BOTTOM, false);

    //Get the bottom screen's frame buffer
    u8* fb = gfxGetFramebuffer(GFX_TOP, GFX_RIGHT, NULL, NULL);

    u8* bottom_fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_RIGHT, NULL, NULL);
    memcpy(bottom_fb, logo_bgr, logo_bgr_size);

    World* w = new World(new Vector2(WIDTH, HEIGTH));
    w->GenerateCells();
    w->PopulateNeighbourgs();
    w->Compute();

    int frameCount = 0;
	// Main loop
	while (aptMainLoop())
	{
        w->Print();
        w->Compute();

        //Copy our image in the bottom screen's frame buffer
        memcpy(fb, world_framebuffer, world_framebuffer_size);

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
