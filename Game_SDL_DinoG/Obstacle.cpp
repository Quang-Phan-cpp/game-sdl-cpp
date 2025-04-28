#include "Obstacle.h"

Obstacle::Obstacle(int x, int y, int w, int h) {
    obstacleRect = {x, y, w, h};
    xPos = static_cast<float>(x);
    speed = 350.0f;
    passed = false;
    textureIndex = 0;
}

void Obstacle::update(float deltaTime) {
    xPos -= speed * deltaTime;
    obstacleRect.x = static_cast<int>(xPos);
}

void Obstacle::render(SDL_Renderer* renderer, const std::vector<SDL_Texture*>& textures) {
    if (textureIndex >= 0 && textureIndex < static_cast<int>(textures.size())) {
        SDL_RenderCopy(renderer, textures[textureIndex], nullptr, &obstacleRect);
    }
}

SDL_Rect Obstacle::getRect() const {
    return obstacleRect;
}

void Obstacle::increaseSpeed() {
    if( speed<=1000 ) speed += 50.0f;
}

float Obstacle::getSpeed() const {
    return speed;
}

bool Obstacle::isPassed() const {
    return passed;
}

void Obstacle::setPassed(bool val) {
    passed = val;
}

void Obstacle::setTextureIndex(int index) {
    textureIndex = index;
}

void Obstacle::setX(float newX) {
    xPos = newX;
    obstacleRect.x = static_cast<int>(xPos);
}
