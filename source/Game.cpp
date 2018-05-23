#include <3ds.h>
#include <stdio.h>
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
    RenderBottomScreen();

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
        FlushBuffer();

        gspWaitForVBlank();
    }

    gfxExit();
}

void Game::InitializeSystem()
{
    gfxInitDefault();

    gfxSetDoubleBuffering(GFX_TOP, false);
    gfxSetDoubleBuffering(GFX_BOTTOM, false);

    top_framebuffer = gfxGetFramebuffer(GFX_TOP, GFX_RIGHT, NULL, NULL);
    bottom_framebuffer = gfxGetFramebuffer(GFX_BOTTOM, GFX_RIGHT, NULL, NULL);
}

void Game::InitializeUnivers()
{
    universe = new Universe(new Vector2(WIDTH, HEIGTH));
}

void Game::HandleInputs()
{
    hidScanInput();

    u32 kDown = hidKeysDown();
    u32 kHeld = hidKeysHeld();
    u32 kUp = hidKeysUp();

    if (kDown & KEY_SELECT) run = 0;

    if (kDown != kDownOld || kHeld != kHeldOld || kUp != kUpOld)
    {
        if (kDown & KEY_START)
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

void Game::FlushBuffer()
{
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