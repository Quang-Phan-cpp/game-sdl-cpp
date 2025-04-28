#include "Dino.h"
#include <SDL_image.h>
#include <iostream>

Dino::Dino() {
    dinoRect = {80, GROUND_Y, 70, 75}; // Vị trí và kích thước khủng long
    posY = (float)GROUND_Y;
    velocityY = 0.0f;
    isJumping = false;

    currentFrame = 0;
    lastFrameTime = SDL_GetTicks();
    frameDelay = 100;
}

Dino::~Dino() {
    for (auto tex : runTextures) {
        if (tex) SDL_DestroyTexture(tex);
    }
    runTextures.clear();
}

void Dino::handleEvent(SDL_Event& e) {
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
        if (!isJumping) {
            velocityY = -600.0f;
            isJumping = true;
        }
    }
}

void Dino::update(float deltaTime) {
    velocityY += 1500.0f * deltaTime;
    posY += velocityY * deltaTime;

    if (posY >= GROUND_Y) {
        posY = GROUND_Y;
        velocityY = 0.0f;
        isJumping = false;
    }

    dinoRect.y = (int)posY;

    if (!isJumping) {
        updateAnimation(deltaTime);
    }
}

void Dino::render(SDL_Renderer* renderer) {
    if (runTextures.empty()) {
        std::cout << "runTextures is empty!\n";
        return;
    }

    if (currentFrame < 0 || currentFrame >= (int)runTextures.size()) currentFrame = 0;

    SDL_Texture* tex = isJumping ? runTextures[0] : runTextures[currentFrame];

    if (!tex) {
        std::cout << "Current texture is null at frame: " << currentFrame << std::endl;
        return;
    }

    SDL_RenderCopy(renderer, tex, nullptr, &dinoRect);
}

SDL_Rect Dino::getRect() {
    return dinoRect;
}

void Dino::loadRunTextures(SDL_Renderer* renderer) {
    // Xóa các texture cũ nếu có
    for (auto tex : runTextures) {
        if (tex) SDL_DestroyTexture(tex);
    }
    runTextures.clear();

    // Load texture mới
    const char* texturePaths[2] = {
        "./assets/images/dino4.png",
        "./assets/images/dino2.png"
    };

    for (int i = 0; i < 2; ++i) {
        SDL_Surface* surface = IMG_Load(texturePaths[i]);
        if (!surface) {
            std::cout << "Failed to load " << texturePaths[i] << ": " << IMG_GetError() << std::endl;
            runTextures.push_back(nullptr);
            continue;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        if (!texture) {
            std::cout << "Failed to create texture from " << texturePaths[i] << ": " << SDL_GetError() << std::endl;
        }

        runTextures.push_back(texture);
    }

    // Debug
    std::cout << "Loaded dino textures. Tex0: " << runTextures[0] << ", Tex1: " << runTextures[1] << std::endl;
}

void Dino::updateAnimation(float deltaTime) {
    if (!isJumping) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime > lastFrameTime + frameDelay) {
            currentFrame = (currentFrame + 1) % 2; // Chuyển giữa 0 và 1
            lastFrameTime = currentTime;
        }
    }
}

