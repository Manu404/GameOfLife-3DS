#ifndef GAME_H
#define GAME_H

#include <3ds/services/am.h>
#include "Universe.h"
#include "Color.h"

#define SOC_ALIGN       0x1000
#define SOC_BUFFERSIZE  0x100000

static u32 *SOC_buffer = NULL;

class Game
{
    Color* Foreground;
    Color* Background;

    u8* top_framebuffer, *bottom_framebuffer;
    Universe* universe;
    u32 kDownOld = 0, kHeldOld = 0, kUpOld = 0; //In these variables there will be information about keys detected in the previous frame
    int frameCount = 0, speedFactor = 1, run = 1, animateForeground = 0, animateBackground = 0, zoomFactor = 1, viewPortX = UNIVERSE_WIDTH, viewPortY = UNIVERSE_HEIGHT, scrollSpeed = 1;

    void InitializeUnivers();
    void InitializeSystem();
    void InitializeColors();
    void HandleRandomFactor(u32 kHeld);
    void HandleColor(u32 kDown);
    void HandleZoom(u32 kDown);
    void HandleDirection(u32 kHeld);
    void HandleSpeed(u32 kDown);

    void HandleInputs();
    void FlushBuffer();
    void RenderTopScreen();
    void RenderBottomScreen();
    
public:
    Game();
    void AnimateColor();
    void RunMainLoop();
};
#endif
