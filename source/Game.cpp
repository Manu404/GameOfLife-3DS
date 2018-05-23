#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <logo_bgr.h>
#include "Universe.h"
#include "Vector2.h"
#include "Globals.h"
#include "Game.h"

Game::Game()
{
    InitializeSystem();
    InitializeUnivers();
}

void Game::RunMainLoop()
{
    // Main loop
    while (aptMainLoop() && run)
    {
        printf("ok");
        frameCount += 1;
        universe->Print();

        if (frameCount % speed == 0)
            universe->Compute();

        HandleInputs();

        RenderTopScreen();
        RenderBottomScreen();
        FlushBuffer();

        gspWaitForVBlank();
    }

    gfxExit();
}

void Game::InitializeUnivers()
{
    universe = new Universe(new Vector2(WIDTH, HEIGTH));
    universe->GenerateCells();
    universe->PopulateNeighbourgs();
    universe->Compute();
}

void Game::HandleInputs()
{
    //Scan all the inputs. This should be done once for each frame
    hidScanInput();

    //hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
    u32 kDown = hidKeysDown();
    //hidKeysHeld returns information about which buttons have are held down in this frame
    u32 kHeld = hidKeysHeld();
    //hidKeysUp returns information about which buttons have been just released
    u32 kUp = hidKeysUp();

    if (kDown & KEY_START) run = 0;

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
}

void Game::InitializeSystem()
{
    gfxInitDefault();

    //Initialize console on top screen. Using NULL as the second argument tells the console library to use the internal console structure as current one
    //consoleInit(GFX_BOTTOM, NULL);

    //We don't need double buffering in this example. In this way we can draw our image only once on screen.
    gfxSetDoubleBuffering(GFX_TOP, false);
    gfxSetDoubleBuffering(GFX_BOTTOM, false);

    //Get the bottom screen's frame buffer
    top_framebuffer = gfxGetFramebuffer(GFX_TOP, GFX_RIGHT, NULL, NULL);
    bottom_framebuffer = gfxGetFramebuffer(GFX_BOTTOM, GFX_RIGHT, NULL, NULL);

}

void Game::FlushBuffer()
{
    // Flush and swap framebuffers
    gfxFlushBuffers();
    gfxSwapBuffers();
}

void Game::RenderTopScreen()
{
    memcpy(top_framebuffer, &(universe->universe_framebuffer), universe->world_framebuffer_size);
}

void Game::RenderBottomScreen()
{
    memcpy(bottom_framebuffer, logo_bgr, logo_bgr_size);
}