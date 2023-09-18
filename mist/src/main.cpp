#define SDL_MAIN_HANDLED

#include <iostream>
#include <SDL.h>

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);
    std::cout << "Hello, world!" << std::endl;
    SDL_Quit();
    return 0;
}