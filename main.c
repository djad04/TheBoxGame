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
    TTF_Font* font = NULL;
    SDL_Texture* playerTexture = NULL;
    SDL_Texture* boxTexture = NULL;
    SDL_Surface* tempSurface = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        erreur("SDL_Init", NULL, NULL);
        return 1;
    }
  
    if (TTF_Init() == -1) {
        erreur("TTF_Init", NULL, NULL);
        return 1;
    }

   
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        erreur("IMG_Init", NULL, NULL);
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

     // Load player image (sprite sheet)
    tempSurface = IMG_Load("character.png");
    if (tempSurface == NULL) {
        erreur("Failed to load devtous.png", window, renderer);
        return 1;
    }
    playerTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
    if (playerTexture == NULL) {
        erreur("Failed to create player texture", window, renderer);
        return 1;
    }

    // Load box image
    tempSurface = IMG_Load("caisse.png");
    if (tempSurface == NULL) {
        erreur("Failed to load caisse.png", window, renderer);
        return 1;
    }
    boxTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
    if (boxTexture == NULL) {
        erreur("Failed to create box texture", window, renderer);
        return 1;
    }

    // Load font
    font = TTF_OpenFont("WONDERKID.ttf", 48);
    if (font == NULL) {
        erreur("TTF_OpenFont", window, renderer);
        return 1;
    }


    bool running = true;
    SDL_Event event;
    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    Uint32 lastKeyPress = 0;
    const Uint32 KEY_DELAY = 150; 

    while (running) {
        




      
    }


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0 ;
}