#include "renderer.h"
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


namespace renderer{
// screen dimensions
const int ScWidth = 1024;
const int ScHeight = 512;
const int xmax = 64;
const int ymax = 32;
SDL_Window* SimonWind = NULL;
SDL_Renderer* SimonRend = NULL;
SDL_Texture* SimScreen = NULL;
unsigned char disp[64 * 32];
unsigned int framebuffer [64 * 32];

bool success = true;
void cldisp () {
    for (int i = 0; i < (64 * 32); i++) {
        disp[i] = 0;
    }
}
bool draw(int x, int y, char b) {
    x &= 63;
    y &= 31;
    bool doot = false;
    if ((disp[x + (y * 64)] == 1) & (b == 1)) {
        doot = true;
    }
    disp[x + (y * 64)] ^= b;
    return doot;
}
bool init() {
    	if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		printf( "Could not initialize SDL. SDL Error: %s\n", SDL_GetError() );
		success = false;
	} else {
        if (SDL_CreateWindowAndRenderer(xmax, ymax, 0, &SimonWind, &SimonRend) < 0) {
            printf("Failed to create window and renderer\n");
            exit(0);
        }

        SDL_SetWindowSize(SimonWind, ScWidth, ScHeight);
        SDL_RenderSetLogicalSize(SimonRend, ScWidth, ScHeight);
        SDL_SetWindowResizable(SimonWind, SDL_FALSE);
        SDL_SetWindowTitle(SimonWind, "Simon 8");

        SimScreen = SDL_CreateTexture(SimonRend, SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STREAMING, xmax, ymax);
    }
    return success;
}

void refresh () {
    int pitch = (64 * 4);
    for (int p = 0; p < (64 * 32); p++) {
        if (disp[p] == 1) {
            framebuffer[p] = 0xFFFFFFFF;
        } else {
            framebuffer[p] = 0x00000000;
        }
    }
    SDL_UpdateTexture(SimScreen, NULL, framebuffer, pitch);
    SDL_RenderClear( SimonRend );
    SDL_RenderCopy(SimonRend,SimScreen, NULL, NULL);
    SDL_RenderPresent(SimonRend);

}

void close() {
    //Free loaded image
    SDL_DestroyTexture(SimScreen);
    SimScreen = NULL;

    //Destroy window    
    SDL_DestroyRenderer( SimonRend );
    SDL_DestroyWindow( SimonWind );
    SimonRend = NULL;
    SimonWind = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}

}

