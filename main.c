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
#define PLAYER_ROWS 4
#define PLAYER_ANIM_SPEED_MS 120

typedef enum {
    DIR_UP = 0,
    DIR_LEFT = 1,
    DIR_DOWN = 2,
    DIR_RIGHT = 3
} PlayerDir;

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

static void addWall(Level* level, int x, int y, int w, int h) {
    if (level->wallCount >= MAX_WALLS) {
        return;
    }
    level->walls[level->wallCount++].rect = (SDL_Rect){ x, y, w, h };
}




void erreur(char* message, SDL_Window* window, SDL_Renderer* renderer);
void drawGame(SDL_Renderer* renderer, SDL_Rect* player, SDL_Rect* box, SDL_Rect* playerSrc, Level* level, TTF_Font* font, int currentLevel, SDL_Texture* playerTex, SDL_Texture* boxTex);
bool checkCollision(SDL_Rect a, SDL_Rect b);
bool checkWallCollision(SDL_Rect* rect, Level* level);
void movePlayer(SDL_Rect* player, SDL_Rect* box, int dx, int dy, Level* level);
void initLevel1(Level* level);

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
        erreur("Failed to load character.png", window, renderer);
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

       // Player sprite animation state
    int playerTexW = 0, playerTexH = 0;
    SDL_QueryTexture(playerTexture, NULL, NULL, &playerTexW, &playerTexH);
    int playerFrameH = (PLAYER_ROWS > 0) ? (playerTexH / PLAYER_ROWS) : playerTexH;
    if (playerFrameH <= 0) playerFrameH = playerTexH;
    int playerFrameW = playerFrameH; // assume square frames
    int playerNumCols = (playerFrameW > 0) ? (playerTexW / playerFrameW) : 1;
    if (playerNumCols <= 0) playerNumCols = 1;
    PlayerDir playerDir = DIR_DOWN;
    int playerFrameIndex = 0;
    Uint32 lastAnimTick = 0;
    SDL_Rect playerSrc = (SDL_Rect){ 0, playerFrameH * playerDir, playerFrameW, playerFrameH };
    bool isMoving = false;

       // Game variables
    int currentLevel = 1;
    Level level1;
    Level* currentLevelPtr = &level1;

    // Player and box
    SDL_Rect player, box;


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

                        currentLevel = 1;
                        currentLevelPtr = &level1;
                        player = currentLevelPtr->playerStart;
                        box = currentLevelPtr->boxStart;
                        playerDir = DIR_DOWN;
                        playerFrameIndex = 0;
                        playerSrc.y = playerFrameH * playerDir;
                        playerSrc.x = 0;

    // 3. Clear the screen
    SDL_SetRenderDrawColor(renderer, 18, 18, 0, 255); // black background
    SDL_RenderClear(renderer);

    // 4. Draw everything
    drawGame(renderer, &player, &box, &playerSrc, currentLevelPtr, font, currentLevel, playerTexture, boxTexture);

    // 5. Present the renderer
    SDL_RenderPresent(renderer);

    // 6. Delay or cap frame rate
    SDL_Delay(16); // ~60 FPS

       }

    SDL_RenderPresent(renderer);
    }


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0 ;
}




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
void drawGame(SDL_Renderer* renderer, SDL_Rect* player, SDL_Rect* box, SDL_Rect* playerSrc, Level* level, TTF_Font* font, int currentLevel, SDL_Texture* playerTex, SDL_Texture* boxTex) {
    // Draw walls
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    for (int i = 0; i < level->wallCount; i++) {
        SDL_RenderFillRect(renderer, &level->walls[i].rect);
    }

    // Draw target (green square)
    SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
    SDL_RenderFillRect(renderer, &level->target);

    // Draw box (using image)
    SDL_RenderCopy(renderer, boxTex, NULL, box);

    // Draw player (sprite sheet frame)
    SDL_RenderCopy(renderer, playerTex, playerSrc, player);

    // Draw level info
    SDL_Color white = { 255, 255, 255, 255 };
    char levelText[50];
    sprintf(levelText, "Level %d", currentLevel);

    TTF_Font* smallFont = TTF_OpenFont("WONDERKID.ttf", 32);
    if (smallFont) {
        SDL_Surface* levelSurface = TTF_RenderText_Solid(smallFont, levelText, white);
        SDL_Texture* levelTexture = SDL_CreateTextureFromSurface(renderer, levelSurface);
        SDL_Rect levelRect = { 10, 10, levelSurface->w, levelSurface->h };
        SDL_RenderCopy(renderer, levelTexture, NULL, &levelRect);
        SDL_FreeSurface(levelSurface);
        SDL_DestroyTexture(levelTexture);

        // Instructions
        SDL_Surface* instrSurface = TTF_RenderText_Solid(smallFont, "R: Reset  ESC: Menu", white);
        SDL_Texture* instrTexture = SDL_CreateTextureFromSurface(renderer, instrSurface);
        SDL_Rect instrRect = { 10, 50, instrSurface->w, instrSurface->h };
        SDL_RenderCopy(renderer, instrTexture, NULL, &instrRect);
        SDL_FreeSurface(instrSurface);
        SDL_DestroyTexture(instrTexture);

        TTF_CloseFont(smallFont);
    }
}
bool checkCollision(SDL_Rect a, SDL_Rect b) {
    return (a.x < b.x + b.w && a.x + a.w > b.x &&
        a.y < b.y + b.h && a.y + a.h > b.y);
}
bool checkWallCollision(SDL_Rect* rect, Level* level) {
    for (int i = 0; i < level->wallCount; i++) {
        if (checkCollision(*rect, level->walls[i].rect)) {
            return true;
        }
    }
    return false;
}

void initLevel1(Level* level) {
    level->wallCount = 0;

    // Create border walls
    // Top wall
    for (int i = 0; i < WINDOW_WIDTH; i += TILE_SIZE) {
        addWall(level, i, 0, TILE_SIZE, TILE_SIZE);
    }
    // Bottom wall
    for (int i = 0; i < WINDOW_WIDTH; i += TILE_SIZE) {
        addWall(level, i, WINDOW_HEIGHT - TILE_SIZE, TILE_SIZE, TILE_SIZE);
    }
    // Left wall
    for (int i = TILE_SIZE; i < WINDOW_HEIGHT - TILE_SIZE; i += TILE_SIZE) {
        addWall(level, 0, i, TILE_SIZE, TILE_SIZE);
    }
    // Right wall
    for (int i = TILE_SIZE; i < WINDOW_HEIGHT - TILE_SIZE; i += TILE_SIZE) {
        addWall(level, WINDOW_WIDTH - TILE_SIZE, i, TILE_SIZE, TILE_SIZE);
    }

    // Add some obstacles
    addWall(level, 200, 200, TILE_SIZE, TILE_SIZE);
    addWall(level, 200, 250, TILE_SIZE, TILE_SIZE);
    addWall(level, 400, 300, TILE_SIZE, TILE_SIZE);
    addWall(level, 450, 300, TILE_SIZE, TILE_SIZE);

    // Set target position (green square)
    level->target = (SDL_Rect){ 650, 450, TILE_SIZE, TILE_SIZE };

    // Set starting positions
    level->playerStart = (SDL_Rect){ 100, 100, 40, 40 };
    level->boxStart = (SDL_Rect){ 300, 150, TILE_SIZE, TILE_SIZE };
}