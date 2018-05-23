#ifndef GAME_H
#define GAME_H

#include <3ds/services/am.h>
#include "Universe.h"

class Game
{
    u8* top_framebuffer, *bottom_framebuffer;
    Universe* universe;
    u32 kDownOld = 0, kHeldOld = 0, kUpOld = 0; //In these variables there will be information about keys detected in the previous frame
    int frameCount = 0;
    int speed = 1;
    int run = 1;

    void InitializeUnivers();
    void HandleInputs();
    void InitializeSystem();
    void FlushBuffer();
    void RenderTopScreen();
    void RenderBottomScreen();

public:
    Game();
    void RunMainLoop();

};
#endif
