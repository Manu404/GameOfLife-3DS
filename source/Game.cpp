#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <list>
#include <logo_bgr.h>
#include "Universe.h"
#include "Vector2.h"
#include "Globals.h"
#include "Game.h"
#include "opc_client.h"
#include <malloc.h>

Game::Game()
{
    InitializeSystem();
    //InitializeOPCClient();
    InitializeUnivers();
    InitializeColors();
}

void Game::RunMainLoop()
{
    RenderBottomScreen();
    
    // Main loop
    while (aptMainLoop() && run)
    {
        HandleInputs();

        AnimateColor();

        universe->Print(this->Foreground, this->Background, zoomFactor, viewPortX, viewPortY);

        if (frameCount % speedFactor == 0)
            universe->Compute();

        //SendOPCFrame();

        RenderTopScreen();

        FlushBuffer();

        frameCount++;

        gspWaitForVBlank();
    }

    gfxExit();
}

void Game::SendOPCFrame()
{
    BuildOPCFrame();

    this->client->write(frameBuffer);
}

void Game::BuildOPCFrame()
{
    uint8_t *dest = OPCClient::Header::view(frameBuffer).data();
    int shift = 0;
    for (int i = 0; i < UNIVERSE_HEIGHT * UNIVERSE_WIDTH; i++) {
        if (((i != 0) && (i % 64 == 60 || i % 64 == 61 || i % 64 == 62 || i % 64 == 63))) {
            *(dest++) = 255;
            *(dest++) = 0;
            *(dest++) = 0;
            shift += 1;
        }
        else {
            int adjustedI = i - shift;
            int currentY = ((((adjustedI / 20) / 6) * 5) + ((adjustedI % 20) / 4));
            int currentX = ((((adjustedI / 20) % 6) * 4) + ((adjustedI % 20) % 4));

            if ((((i - shift) % 20) / 4) % 2 == 1) {
                currentX += 3;
            }

            int invertedX = UNIVERSE_WIDTH - 1 - currentX;
            int invertedY = UNIVERSE_HEIGHT  - 1 - currentY;

            int offset = ((invertedX * (UNIVERSE_HEIGHT)) + (UNIVERSE_HEIGHT - invertedY - 1)) * 3;

            *(dest++) = universe->universe_framebuffer_opc[offset];
            *(dest++) = universe->universe_framebuffer_opc[offset + 1];
            *(dest++) = universe->universe_framebuffer_opc[offset + 2];
        }
    }
}


void Game::InitializeOPCClient()
{    
    SOC_buffer = (u32*)memalign(SOC_ALIGN, SOC_BUFFERSIZE);
    int ret = -1;

    if ((ret = socInit(SOC_buffer, SOC_BUFFERSIZE)) != 0) {
        if(DEBUG)
            printf("socInit: 0x%08X\n", (unsigned int)ret);
    }

    this->client = new OPCClient();
    if(HSLG)
        this->client->resolve("192.168.42.64", 7890);
    else
        this->client->resolve("192.168.1.62", 7890);

    header = new OPCClient::Header();
    int frameBytes = (UNIVERSE_WIDTH * UNIVERSE_HEIGHT) * 3;
    frameBuffer.resize(sizeof(OPCClient::Header) + frameBytes);
    OPCClient::Header::view(frameBuffer).init(HSLG ? 0 : 1, this->client->SET_PIXEL_COLORS, frameBytes);
}

void Game::InitializeSystem()
{
    gfxInitDefault();

    gfxSetDoubleBuffering(GFX_TOP, false);
    top_framebuffer = gfxGetFramebuffer(GFX_TOP, GFX_RIGHT, NULL, NULL);

    if (DEBUG)
        consoleInit(GFX_BOTTOM, NULL);
    else
    {
        gfxSetDoubleBuffering(GFX_BOTTOM, false);
        bottom_framebuffer = gfxGetFramebuffer(GFX_BOTTOM, GFX_RIGHT, NULL, NULL);
    }
}

void Game::InitializeUnivers()
{
    universe = new Universe(new Vector2(UNIVERSE_WIDTH, UNIVERSE_HEIGHT));
}

void Game::AnimateColor()
{
    if (this->animateForeground) {
        this->Foreground->NextGradientColor(speedFactor);

        if (this->animateBackground)
            this->Background->ComputeComplementFromColor(this->Foreground);
    }
    else if (this->animateBackground)
        this->Background->NextGradientColor(speedFactor);
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
    u32 kHeld = hidKeysHeld();
    u32 kUp = hidKeysUp();

    if (kDown & KEY_SELECT) {
        run = !run; 
        return;
    }

    if (kDown & KEY_START)
        universe->Reset();

    HandleSpeed(kDown);
    HandleDirection(kHeld);
    HandleZoom(kDown);
    HandleColor(kDown);
    HandleRandomFactor(kHeld);

    kDownOld = kDown;
    kHeldOld = kHeld;
    kUpOld = kUp;
}


void Game::HandleRandomFactor(u32 kHeld)
{
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

void Game::HandleColor(u32 kDown)
{
    if (kDown & KEY_Y) {
        this->animateForeground = !this->animateForeground;
        if (!this->animateForeground)
            this->Foreground = new Color(255, 255, 255);
    }
    else if (kDown & KEY_X) {
        this->animateBackground = !this->animateBackground;
        if (!this->animateBackground)
            this->Background = new Color(0, 0, 0);
    }
}

void Game::HandleZoom(u32 kDown)
{
    if (kDown & KEY_DDOWN) {
        if (zoomFactor > 1)
            zoomFactor--;
    }
    else if (kDown & KEY_DUP) {
        zoomFactor++;

        if (viewPortY > UNIVERSE_HEIGHT - (UNIVERSE_HEIGHT / zoomFactor))
            viewPortY = UNIVERSE_HEIGHT - (UNIVERSE_HEIGHT / zoomFactor);
        if (viewPortX > UNIVERSE_WIDTH - (UNIVERSE_WIDTH / zoomFactor))
            viewPortX = UNIVERSE_WIDTH - (UNIVERSE_WIDTH / zoomFactor);
    }
}

void Game::HandleDirection(u32 kHeld)
{
    if (kHeld & KEY_CPAD_LEFT) {
        if (viewPortX + (scrollSpeed * zoomFactor) > scrollSpeed * zoomFactor)
            viewPortX -= scrollSpeed * zoomFactor;
        else
            viewPortX = 0;
    }
    if (kHeld & KEY_CPAD_RIGHT) {
        if (viewPortX + (scrollSpeed * zoomFactor) < UNIVERSE_WIDTH - (scrollSpeed * zoomFactor) - (UNIVERSE_WIDTH / zoomFactor))
            viewPortX += scrollSpeed * zoomFactor;
        else
            viewPortX = UNIVERSE_WIDTH - (UNIVERSE_WIDTH / zoomFactor);
    }

    if (kHeld & KEY_CPAD_DOWN)
    {
        if (viewPortY + (scrollSpeed * zoomFactor) > scrollSpeed * zoomFactor)
            viewPortY -= scrollSpeed * zoomFactor;
        else
            viewPortY = 0;
    }
    if (kHeld & KEY_CPAD_UP) {
        if (viewPortY + (scrollSpeed * zoomFactor) < UNIVERSE_HEIGHT - (scrollSpeed * zoomFactor) - (UNIVERSE_HEIGHT / zoomFactor))
            viewPortY += scrollSpeed * zoomFactor;
        else
            viewPortY = UNIVERSE_HEIGHT - (UNIVERSE_HEIGHT / zoomFactor);
    }
}

void Game::HandleSpeed(u32 kDown)
{
    if (kDown & KEY_DRIGHT)
    {
        if (speedFactor > 0)
            speedFactor--;
    }
    else if (kDown & KEY_DLEFT) {
        speedFactor++;
    }
}

void Game::FlushBuffer()
{
    gfxFlushBuffers();
    gfxSwapBuffers();
}

void Game::RenderTopScreen()
{
    memcpy(top_framebuffer, (universe->universe_framebuffer), universe->universe_framebuffer_size);
}

void Game::RenderBottomScreen()
{
    if(!DEBUG)
        memcpy(bottom_framebuffer, logo_bgr, logo_bgr_size);
}