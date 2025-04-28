#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_ttf.h>
#include "Dino.h"
#include "Obstacle.h"
#include <vector>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <string>

enum GameState {
    MENU,
    PLAYING,
    GAME_OVER
};

class Game {
public:
    Game();
    ~Game();
    bool init();
    void run();
    void close();
    bool checkCollision(SDL_Rect a, SDL_Rect b);
    int loadHighScore();
    void saveHighScore(int hs);
    void resetGame();

private:
    GameState gameState;
    std::vector<Obstacle> obstacles;
    const int OBSTACLE_SPACING = 900;
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    Dino* dino;
    int score;
    int lastScoreMilestone;
    int highScore;

    float bgX;
    float bgSpeed;

    TTF_Font* font;

    SDL_Texture* dinoTexture;
    std::vector<SDL_Texture*> obstacleTextures;
    SDL_Texture* backgroundTexture;
    SDL_Texture* menuBackgroundTexture;

    void renderScore();
    SDL_Texture* loadTexture(const char* path);

    Mix_Chunk* jumpSound;
    Mix_Chunk* hitSound;
    Mix_Music* bgMusic;
    Mix_Music* menuMusic;

    void renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y);
};

#endif
