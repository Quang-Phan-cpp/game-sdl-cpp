#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <SDL.h>
#include "Constants.h"
#include <vector>

class Obstacle {
public:
    Obstacle(int x, int y, int w, int h);

    void update(float deltaTime); 
    void render(SDL_Renderer* renderer, const std::vector<SDL_Texture*>& textures);

    SDL_Rect getRect() const;      
    float getSpeed() const;       
    bool isPassed() const;         

    void increaseSpeed();          
    void setPassed(bool val);      
    void setTextureIndex(int index); 
    void setX(float newX);         

private:
    SDL_Rect obstacleRect; 
    float xPos;            
    float speed;           
    bool passed;           
    int textureIndex;     
};

#endif // OBSTACLE_H
