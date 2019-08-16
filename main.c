#define SDL_MAIN_HANDLED
#include "sdl2/include/SDL.h"
#include "sdl2/include/SDL_image.h"
#include "sdl2/include/SDL_ttf.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define PLAYER_SPEED 5
#define TILE_SIZE 50
#define MAX_WALLS 256
#define PLAYER_HITBOX_INSET_X 10
#define PLAYER_HITBOX_INSET_Y 6

// Wall structure
typedef struct {
    SDL_Rect rect;
} Wall;

// Level structure
typedef struct {
    Wall walls[MAX_WALLS];
    int wallCount;
    SDL_Rect target;
    SDL_Rect playerStart;
    SDL_Rect boxStart;
} Level;

void erreur(char* message, SDL_Window* window, SDL_Renderer* renderer) {
    char errorMsg[512];
    sprintf(errorMsg, "ERROR %s: %s\n\nPress any key to exit...", message, SDL_GetError());
    SDL_Log("%s", errorMsg);

    // Show error in message box
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game Error", errorMsg, NULL);

    if (renderer != NULL) SDL_DestroyRenderer(renderer);
    if (window != NULL) SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    exit(EXIT_FAILURE);
}

void movePlayer(SDL_Rect* player, SDL_Rect* box, int dx, int dy, Level* level) {
    SDL_Rect newPlayerPos = *player;
    // Construct a tighter hitbox for collision tests
    SDL_Rect playerHitbox = {
        newPlayerPos.x + PLAYER_HITBOX_INSET_X,
        newPlayerPos.y + PLAYER_HITBOX_INSET_Y,
        newPlayerPos.w - 2 * PLAYER_HITBOX_INSET_X,
        newPlayerPos.h - 2 * PLAYER_HITBOX_INSET_Y
    };
    newPlayerPos.x += dx;
    newPlayerPos.y += dy;
    SDL_Rect newPlayerHitbox = {
        newPlayerPos.x + PLAYER_HITBOX_INSET_X,
        newPlayerPos.y + PLAYER_HITBOX_INSET_Y,
        newPlayerPos.w - 2 * PLAYER_HITBOX_INSET_X,
        newPlayerPos.h - 2 * PLAYER_HITBOX_INSET_Y
    };

    // Check if player would collide with walls
    if (checkWallCollision(&newPlayerHitbox, level)) {
        return;
    }

    // Check if player would collide with box
    if (checkCollision(newPlayerHitbox, *box)) {
        SDL_Rect newBoxPos = *box;
        newBoxPos.x += dx;
        newBoxPos.y += dy;

        // Check if box would collide with walls
        if (checkWallCollision(&newBoxPos, level)) {
            return;
        }

        // Check screen boundaries for box
        if (newBoxPos.x < 0 || newBoxPos.x + newBoxPos.w > WINDOW_WIDTH ||
            newBoxPos.y < 0 || newBoxPos.y + newBoxPos.h > WINDOW_HEIGHT) {
            return;
        }

        // Move box
        box->x = newBoxPos.x;
        box->y = newBoxPos.y;
    }

    // Check screen boundaries for player
    if (newPlayerHitbox.x < 0 || newPlayerHitbox.x + newPlayerHitbox.w > WINDOW_WIDTH ||
        newPlayerHitbox.y < 0 || newPlayerHitbox.y + newPlayerHitbox.h > WINDOW_HEIGHT) {
        return;
    }

    // Move player
    player->x = newPlayerPos.x;
    player->y = newPlayerPos.y;
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
        
       while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }


       }

    }


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0 ;
}