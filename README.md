# The Box Game

A puzzle game where you push boxes to solve levels. Built with C and SDL2.

## Building and Running

1. Create a `build` folder in the project root
2. Copy all DLL files from `sdl2/bin` to the `build` folder
3. Run this command to build:
```bash
gcc main.c -o build/main.exe -I sdl2/include -L sdl2/lib -lSDL2 -lSDL2main -lSDL2_image -lSDL2_ttf
```
4. Run the game: `build/main.exe`

## Controls
- Arrow keys: Move player
- R: Reset level
- Esc: Menu

## License
MIT License 