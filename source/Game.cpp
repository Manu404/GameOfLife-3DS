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
    InitializeColors();
}

void Game::RunMainLoop()
{
    RenderBottomScreen();

    // Main loop
    while (aptMainLoop() && run)
    {
        frameCount++;

        if (this->animateForeground)
            this->Foreground->NextRGB(speedFactor);

        if (this->animateBackground)
            this->Background->ComputeComplementFromColor(this->Foreground);

        universe->Print(this->Foreground, this->Background);

        if (frameCount % speedFactor == 0)
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

void Game::InitializeColors()
{
    this->Foreground = new Color(255, 255, 255);
    this->Background = new Color(0, 0, 0);
}

void Game::HandleInputs()
{
    hidScanInput();

    u32 kDown = hidKeysDown();
    if (kDown & KEY_SELECT) {
        run = !run; 
        return;
    }
    
    u32 kHeld = hidKeysHeld();
    u32 kUp = hidKeysUp();

    if (kDown != kDownOld || kHeld != kHeldOld || kUp != kUpOld)
    {
        if (kDown & KEY_START)
            universe->Reset();

        if (kHeld & KEY_DUP || kHeld & KEY_DRIGHT)
        {
            if (speedFactor > 0)
                speedFactor--;
        }
        else if (kHeld & KEY_DDOWN || kHeld & KEY_DLEFT)
            speedFactor++;


        if (kDown & KEY_Y) {
            this->animateForeground = !this->animateForeground;
            if(!this->animateForeground)
                this->Foreground = new Color(255, 255, 255);
        }

        if (kDown & KEY_X) {
            this->animateBackground = !this->animateBackground;
            if(!this->animateBackground)
                this->Background = new Color(0, 0, 0);
        }

        if (kHeld & KEY_L) {
            universe->RandomFactor++;
            universe->Reset();
        }
        else if (kHeld & KEY_R) {
            if (universe->RandomFactor > 2) {
                universe->RandomFactor--;
            }
            universe->Reset();
        }
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