#ifndef DINO_H
#define DINO_H

#include <SDL.h>
#include "Constants.h"
#include <vector>

#define GROUND_Y 247

class Dino {
public:
    Dino();
    ~Dino();

    void handleEvent(SDL_Event& e);
    void update(float deltaTime);
    void render(SDL_Renderer* renderer);
    SDL_Rect getRect();
    void loadRunTextures(SDL_Renderer* renderer);

private:
    SDL_Rect dinoRect;
    bool isJumping;
    float velocityY;
    float posY;

    std::vector<SDL_Texture*> runTextures;
    int currentFrame;
    Uint32 lastFrameTime;
    int frameDelay;

    void updateAnimation(float deltaTime);
};

#endif
