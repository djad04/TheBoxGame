#define SDL_MAIN_HANDLED
#include "sdl2/include/SDL.h"
#include "sdl2/include/SDL_image.h"
#include "sdl2/include/SDL_ttf.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

void erreur(const char *msg, SDL_Window *win, SDL_Renderer *rend) {
    if (msg) fprintf(stderr, "%s: %s\n", msg, SDL_GetError());
    if (rend) SDL_DestroyRenderer(rend);
    if (win) SDL_DestroyWindow(win);
    SDL_Quit();
    exit(1);
}


int main(){

     SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        erreur("SDL_Init", NULL, NULL);
        return 1;
    }

    window = SDL_CreateWindow("Box Puzzle Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN);
    if (window == NULL) {
        erreur("SDL_CreateWindow", NULL, NULL);
        return 1;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        erreur("SDL_CreateRenderer", window, NULL);
        return 1;
    }




    return 0 ;
}