/*
	Hello World example made by Aurelio Mannara for ctrulib
	This code was modified for the last time on: 12/13/2014 01:00 UTC+1

	This wouldn't be possible without the amazing work done by:
	-Smealum
	-fincs
	-WinterMute
	-yellows8
	-plutoo
	-mtheall
	-Many others who worked on 3DS and I'm surely forgetting about
*/

#include <3ds.h>
#include <stdio.h>
#include <string.h>

//This include a header containing definitions of our image
#define imageSize 96000

u8 brew2_bgr[imageSize * 3];
u32 brew2_bgr_size = imageSize * 3;

int main(int argc, char **argv)
{
	gfxInitDefault();

	//Initialize console on top screen. Using NULL as the second argument tells the console library to use the internal console structure as current one
	consoleInit(GFX_BOTTOM, NULL);

	printf("Why so sad Smealum? We can haz 3DS homebrew!");

	printf("\x1b[11;16HPress Start to exit.");

	//We don't need double buffering in this example. In this way we can draw our image only once on screen.
	gfxSetDoubleBuffering(GFX_TOP, false);
	
	for(int i = 0; i < imageSize; i++)
	{
		int color = 0;
		if (i % 2 == 0) color = 255;
		brew2_bgr[i*3] = color;
		brew2_bgr[i*3+1] = color;
		brew2_bgr[i*3+2] = color;
	}

	//Get the bottom screen's frame buffer
	u8* fb = gfxGetFramebuffer(GFX_TOP, GFX_RIGHT, NULL, NULL);
	
	//Copy our image in the bottom screen's frame buffer
	memcpy(fb, brew2_bgr, brew2_bgr_size);

	// Main loop
	while (aptMainLoop())
	{
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
