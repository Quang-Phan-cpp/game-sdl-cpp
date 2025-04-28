#include "Game.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

Game::Game() : window(nullptr), renderer(nullptr), running(false), dino(nullptr), lastScoreMilestone(0) {
    score = 0;
    highScore = loadHighScore();
}

Game::~Game() {
    close();
}

bool Game::init() {
    bgSpeed = 350.0f;
    bgX = 0.0f;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Dino Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() == -1) {
        std::cout << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }

    font = TTF_OpenFont("./assets/fonts/font.ttf", 25);
    if (!font) {
        std::cout << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cout << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << std::endl;
        return false;
    }

    backgroundTexture = loadTexture("./assets/images/background2.png");
    obstacleTextures.push_back(loadTexture("./assets/images/tree1.png"));
    obstacleTextures.push_back(loadTexture("./assets/images/tree2.png"));
    menuBackgroundTexture = loadTexture("./assets/images/menu.png");

    if (!backgroundTexture || obstacleTextures.empty() || !menuBackgroundTexture ) {
        std::cout << "Fail to load image!" << std::endl;
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "SDL_mixer could not initialize! Mix_Error: " << Mix_GetError() << std::endl;
        return false;
    }

    jumpSound = Mix_LoadWAV("./assets/sounds/jump.mp3");
    hitSound = Mix_LoadWAV("./assets/sounds/hit.mp3");
    bgMusic = Mix_LoadMUS("./assets/sounds/bg_music1.mp3");
    menuMusic = Mix_LoadMUS("./assets/sounds/menu_music.mp3");

    if (!jumpSound || !hitSound || !bgMusic || !menuMusic) {
        std::cout << "Failed to load sound effects! Mix_Error: " << Mix_GetError() << std::endl;
        return false;
    }

    for (int i = 0; i < 3; ++i) {
        int x = 1500 + i * OBSTACLE_SPACING;
        Obstacle obs(x, SCREEN_HEIGHT - 150, 66, 65);
        obs.setTextureIndex(rand() % obstacleTextures.size());
        obstacles.push_back(obs);
    }

    gameState = MENU;

    dino = new Dino();
    dino->loadRunTextures(renderer);

    return true;
}

void Game::run() {
    running = true;
    SDL_Event event;
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;
    Uint32 frameStart;
    int frameTime;
    float deltaTime = 0;
    Uint32 lastTick = SDL_GetTicks();

    while (running) {
        frameStart = SDL_GetTicks();
        Uint32 currentTick = SDL_GetTicks();
        deltaTime = (currentTick - lastTick) / 1000.0f;
        lastTick = currentTick;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_m) {
                gameState = MENU;
                Mix_HaltMusic();
                Mix_PlayMusic(menuMusic, -1);
            }

            if (gameState == MENU && event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_SPACE) {
                resetGame();
                gameState = PLAYING;
            } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
                }
            }

            dino->handleEvent(event);

            if (gameState == PLAYING && event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
                Mix_PlayChannel(-1, jumpSound, 0);
            }

            if (gameState == GAME_OVER && event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_SPACE) {
                    resetGame();
                } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        if (gameState == MENU) {
            if (!Mix_PlayingMusic()) {
                Mix_PlayMusic(menuMusic, -1);
            }
            SDL_Rect rect = {0, 0, 800, 400};
            SDL_RenderCopy(renderer, menuBackgroundTexture, NULL, &rect);

            renderText(renderer, font, "SPACE to RUN", 320, 188);

            std::string hs = "High Score: " + std::to_string(highScore);
            renderText(renderer, font, hs, 300, 215);

            SDL_RenderPresent(renderer);
            continue;
        }

        if (gameState == PLAYING) {
            bgX -= bgSpeed * deltaTime;
            if (bgX <= -SCREEN_WIDTH) bgX += SCREEN_WIDTH;

            dino->update(deltaTime);
        }

        SDL_Rect bg1 = { static_cast<int>(bgX), 0, SCREEN_WIDTH, SCREEN_HEIGHT };
        SDL_Rect bg2 = { static_cast<int>(bgX) + SCREEN_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
        SDL_RenderCopy(renderer, backgroundTexture, NULL, &bg1);
        SDL_RenderCopy(renderer, backgroundTexture, NULL, &bg2);

        dino->render(renderer);

        if (gameState == PLAYING) {
            for (auto& obstacle : obstacles) {
                obstacle.update(deltaTime);

            if (checkCollision(dino->getRect(), obstacle.getRect())) {
                Mix_PlayChannel(-1, hitSound, 0);
                Mix_HaltMusic();
                if (score > highScore) {
                    highScore = score;
                    saveHighScore(highScore);
                }
                std::cout << "Game Over! Final score: " << score << std::endl;
                gameState = GAME_OVER;
                break;
            }

            if (!obstacle.isPassed() && obstacle.getRect().x + obstacle.getRect().w < dino->getRect().x) {
                score += 10;
                obstacle.setPassed(true);
            }

            if (score > 0 && score % 50 == 0 && lastScoreMilestone != score) {
                lastScoreMilestone = score;
                for (auto& obs : obstacles) {
                    obs.increaseSpeed();
                }
                if (bgSpeed<=1000) bgSpeed+= 50.0f;
            }

            if (obstacle.getRect().x + obstacle.getRect().w < 0) {
                int maxX = 0;
                for (auto& obs : obstacles) {
                    if (obs.getRect().x > maxX)
                        maxX = obs.getRect().x;
            }
            obstacle.setX(maxX + OBSTACLE_SPACING);
            obstacle.setPassed(false);
            obstacle.setTextureIndex(rand() % obstacleTextures.size());
        }
    }
}

for (auto& obstacle : obstacles) {
    obstacle.render(renderer, obstacleTextures);
}

if (gameState == GAME_OVER) {
    SDL_Color textColor = {255, 0, 0, 255};
    SDL_Surface* gameOverSurface = TTF_RenderText_Solid(font, "GAME OVER! SPACE to Playagain", textColor);
    SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);

    SDL_Rect textRect = {
        (SCREEN_WIDTH - gameOverSurface->w) / 2,
        (SCREEN_HEIGHT-50) / 2,
        gameOverSurface->w,
        gameOverSurface->h
    };

    SDL_RenderCopy(renderer, gameOverTexture, NULL, &textRect);
    SDL_FreeSurface(gameOverSurface);
    SDL_DestroyTexture(gameOverTexture);
}

        renderScore();
        SDL_RenderPresent(renderer);

        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
}

void Game::close() {
    SDL_DestroyTexture(backgroundTexture);

    for (auto& tex : obstacleTextures) {
        SDL_DestroyTexture(tex);
    }

    delete dino;

    Mix_FreeChunk(jumpSound);
    Mix_FreeChunk(hitSound);
    Mix_FreeMusic(bgMusic);
    Mix_CloseAudio();

    TTF_CloseFont(font);
    TTF_Quit();

    SDL_DestroyTexture(menuBackgroundTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    IMG_Quit();
}

bool Game::checkCollision(SDL_Rect a, SDL_Rect b) {
    a.x += 10; a.y += 10;
    a.w -= 20; a.h -= 20;
    b.x += 10; b.w -= 20;

    return SDL_HasIntersection(&a, &b);
}

void Game::renderScore() {
    SDL_Color textColor = {0, 128, 0, 255};
    std::string scoreText = "Score: " + std::to_string(score);
    std::string highScoreText = "High Score: " + std::to_string(highScore);

    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    SDL_Surface* highScoreSurface = TTF_RenderText_Solid(font, highScoreText.c_str(), textColor);

    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_Texture* highScoreTexture = SDL_CreateTextureFromSurface(renderer, highScoreSurface);

    SDL_Rect scoreRect = {7, 5, scoreSurface->w, scoreSurface->h};
    SDL_Rect highScoreRect = {7, 30, highScoreSurface->w, highScoreSurface->h};

    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
    SDL_RenderCopy(renderer, highScoreTexture, NULL, &highScoreRect);

    SDL_FreeSurface(scoreSurface);
    SDL_FreeSurface(highScoreSurface);
    SDL_DestroyTexture(scoreTexture);
    SDL_DestroyTexture(highScoreTexture);
}

SDL_Texture* Game::loadTexture(const char* path) {
    SDL_Surface* tempSurface = IMG_Load(path);
    if (!tempSurface) {
        std::cout << "Failed to load " << path << " - IMG Error: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
    return texture;
}

int Game::loadHighScore() {
    std::ifstream in("highscore.txt");
    int hs = 0;
    if (in.is_open()) {
        in >> hs;
        in.close();
    }
    return hs;
}

void Game::saveHighScore(int hs) {
    std::ofstream out("highscore.txt");
    if (out.is_open()) {
        out << hs;
        out.close();
    }
}

void Game::resetGame() {
    score = 0;
    bgSpeed = 350.0f;
    lastScoreMilestone = 0;
    obstacles.clear();

    for (int i = 0; i < 3; ++i) {
        int x = 1500 + i * OBSTACLE_SPACING;
        Obstacle obs(x, SCREEN_HEIGHT - 150, 66, 65);
        obs.setTextureIndex(rand() % obstacleTextures.size());
        obstacles.push_back(obs);
    }

    delete dino;
    dino = new Dino();
    dino->loadRunTextures(renderer);

    Mix_PlayMusic(bgMusic, -1);
    gameState = PLAYING;
}

void Game::renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y) {
    SDL_Color textColor = {0, 140, 0};
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect textRect = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}
