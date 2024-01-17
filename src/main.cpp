#include <SDL2/SDL.h> //OH MA GOD ITS SDL!!1!!11!1!
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <vector>
#include <cstdlib>

//the big rock... they are not friendly
struct Asteroid {
    int x;
    int y;
    int speed;
    bool exploded = false;
};

//the pew pew, they dont like the asteroids
struct Bullet {
    int x;
    int y;
    int speed;
};

//the boom boom, they boom boom
struct Explosion {
    int x;
    int y;
    Uint32 start_time;
};

//the actual game order
enum class GameState {
    Intro,
    Menu,
    Game, //it gets harder
    Death //lol your bad
};

int main(int argc, char* args[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Galex", SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }

    //very pretty noises
    Mix_Chunk* introSound = Mix_LoadWAV("res/sounds/intro.wav");
    Mix_Chunk* shootSound = Mix_LoadWAV("res/sounds/shoot.wav");
    Mix_Chunk* explosionSound = Mix_LoadWAV("res/sounds/explosion.wav");
    Mix_Chunk* DeathSound = Mix_LoadWAV("res/sounds/explosion.wav");


    //drawing pretty pictures
    SDL_Surface* bgSurface = IMG_Load("res/background/bg.png");
    SDL_Texture* bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
    SDL_FreeSurface(bgSurface);

    SDL_Surface* introSurface = IMG_Load("res/menus/intro.png");
    SDL_Texture* introTexture = SDL_CreateTextureFromSurface(renderer, introSurface);
    SDL_FreeSurface(introSurface);

    SDL_Surface* menuSurface = IMG_Load("res/menus/menu.png");
    SDL_Texture* menuTexture = SDL_CreateTextureFromSurface(renderer, menuSurface);
    SDL_FreeSurface(menuSurface);

    SDL_Surface* deathSurface = IMG_Load("res/menus/death.png");
    SDL_Texture* deathTexture = SDL_CreateTextureFromSurface(renderer, deathSurface);
    SDL_FreeSurface(deathSurface);

    SDL_Surface* playerSurface1 = IMG_Load("res/assets/player/spaceship1.png");
    SDL_Texture* playerTexture1 = SDL_CreateTextureFromSurface(renderer, playerSurface1);
    SDL_FreeSurface(playerSurface1);

    SDL_Surface* playerSurface2 = IMG_Load("res/assets/player/spaceship2.png");
    SDL_Texture* playerTexture2 = SDL_CreateTextureFromSurface(renderer, playerSurface2);
    SDL_FreeSurface(playerSurface2);

    SDL_Surface* playerSurface3 = IMG_Load("res/assets/player/spaceship3.png");
    SDL_Texture* playerTexture3 = SDL_CreateTextureFromSurface(renderer, playerSurface3);
    SDL_FreeSurface(playerSurface3);

    SDL_Surface* asteroidSurface = IMG_Load("res/assets/asteroid.png");
    SDL_Texture* asteroidTexture = SDL_CreateTextureFromSurface(renderer, asteroidSurface);
    SDL_FreeSurface(asteroidSurface);

    SDL_Surface* bulletSurface = IMG_Load("res/assets/bullet.png");
    SDL_Texture* bulletTexture = SDL_CreateTextureFromSurface(renderer, bulletSurface);
    SDL_FreeSurface(bulletSurface);

    SDL_Surface* explosionSurface = IMG_Load("res/assets/explode.png");
    SDL_Texture* explosionTexture = SDL_CreateTextureFromSurface(renderer, explosionSurface);
    SDL_FreeSurface(explosionSurface);

    SDL_Surface* numberSurface[10];
    SDL_Texture* numberTexture[10];
    for(int i = 0; i < 10; i++) {
        numberSurface[i] = IMG_Load(("res/font/" + std::to_string(i) + ".png").c_str());
        numberTexture[i] = SDL_CreateTextureFromSurface(renderer, numberSurface[i]);
        SDL_FreeSurface(numberSurface[i]);
    }

    int playerX = 35;
    int playerY = 250;
    int playerSpeed = 5;
    int score = 0;
    float asteroidSpeedIncrease = 0.05;
    int scoreForNextAsteroid = 5;

    std::vector<Asteroid> asteroids(5);
    for (auto& asteroid : asteroids) {
        asteroid.x = 800 + rand() % 600;
        asteroid.y = rand() % 600;
        asteroid.speed = 2 + rand() % 4;
    }

    std::vector<Bullet> bullets;
    std::vector<Explosion> explosions;

    bool isRunning = true;

    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    SDL_Rect playerRect;
    SDL_Rect asteroidRect;
    SDL_Rect bulletRect;

    const int BULLET_DELAY = 300;
    Uint32 lastBulletTime = 0;
    const int EXPLOSION_DURATION = 1000;

    GameState gameState = GameState::Intro;
    Uint32 introStartTime = SDL_GetTicks();

    while (isRunning) { //ze game loopa!1!1!11!
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
                if (gameState == GameState::Menu || gameState == GameState::Death) {
                    gameState = GameState::Game;
                    score = 0;
                    playerX = 35;
                    playerY = 250;
                    asteroids.clear();
                    bullets.clear();
                    explosions.clear();
                    for (int i = 0; i < 5; i++) {
                        Asteroid asteroid;
                        asteroid.x = 800 + rand() % 600;
                        asteroid.y = rand() % 600;
                        asteroid.speed = 2 + rand() % 4;
                        asteroids.push_back(asteroid);
                    }
                }
            }
        }

        Uint32 currentTime = SDL_GetTicks();
        if (gameState == GameState::Intro && currentTime - introStartTime >= 1000) {
            gameState = GameState::Menu;
        }

        SDL_RenderClear(renderer);

        if (gameState == GameState::Intro) {
            SDL_RenderCopy(renderer, introTexture, NULL, NULL);
            Mix_PlayChannel(-1, introSound, 0);
        } else if (gameState == GameState::Menu) {
            SDL_RenderCopy(renderer, menuTexture, NULL, NULL);
        } else if (gameState == GameState::Death) {
            SDL_RenderCopy(renderer, deathTexture, NULL, NULL);
        } else if (gameState == GameState::Game) {

            SDL_RenderCopy(renderer, bgTexture, NULL, NULL);

            if (keystate[SDL_SCANCODE_Q] && playerY - playerSpeed >= 0) {
                playerY -= playerSpeed;
            }
            if (keystate[SDL_SCANCODE_A] && playerY + playerSpeed <= 600 - 96) {
                playerY += playerSpeed;
            }

            if (keystate[SDL_SCANCODE_F] && currentTime > lastBulletTime + BULLET_DELAY) {
                Bullet bullet;
                bullet.x = playerX + 96;
                bullet.y = playerY + 40;
                bullet.speed = 10;
                bullets.push_back(bullet);
                lastBulletTime = currentTime;
                Mix_PlayChannel(-1, shootSound, 0);
            }

            playerRect.x = playerX;
            playerRect.y = playerY;
            playerRect.w = 96;
            playerRect.h = 96;

            if (keystate[SDL_SCANCODE_Q]) {
                SDL_RenderCopy(renderer, playerTexture3, NULL, &playerRect);
            }
            else if (keystate[SDL_SCANCODE_A]) {
                SDL_RenderCopy(renderer, playerTexture2, NULL, &playerRect);
            }
            else {
                SDL_RenderCopy(renderer, playerTexture1, NULL, &playerRect);
            }

            for (auto& asteroid : asteroids) {
                asteroidRect.x = asteroid.x;
                asteroidRect.y = asteroid.y;
                asteroidRect.w = 112;
                asteroidRect.h = 112;
                if (!asteroid.exploded) {
                    SDL_RenderCopy(renderer, asteroidTexture, NULL, &asteroidRect);
                }
                asteroid.x -= asteroid.speed;
                if (asteroid.x <= -64) {
                    asteroid.x = 800 + rand() % 600;
                    asteroid.y = rand() % 600;
                    asteroid.speed = 2 + rand() % 4;
                    asteroid.exploded = false;
                }
                if (!asteroid.exploded && abs(playerX - asteroid.x) < 64 && abs(playerY - asteroid.y) < 64) {
                    Mix_PlayChannel(-1, DeathSound, 0);
                    gameState = GameState::Death;
                    break;
                }
            }

            for (auto it = bullets.begin(); it != bullets.end();) {
                if (it->x > 800) {
                    it = bullets.erase(it);
                } else {
                    bulletRect.x = it->x;
                    bulletRect.y = it->y;
                    bulletRect.w = 32;
                    bulletRect.h = 32;
                    SDL_RenderCopy(renderer, bulletTexture, NULL, &bulletRect);
                    it->x += it->speed;

                    if (it->x >= 0 && it->x <= 800 && it->y >= 0 && it->y <= 600) {
                        for (auto& asteroid : asteroids) {
                            if (asteroid.x >= 0 && asteroid.x <= 800 && asteroid.y >= 0 && asteroid.y <= 600) {
                                if (!asteroid.exploded && abs(it->x - asteroid.x) < 64 && abs(it->y - asteroid.y) < 64) {
                                    asteroid.exploded = true;
                                    Explosion explosion;
                                    explosion.x = asteroid.x;
                                    explosion.y = asteroid.y;
                                    explosion.start_time = currentTime;
                                    explosions.push_back(explosion);
                                    score++;
                                    Mix_PlayChannel(-1, explosionSound, 0);

                                    asteroid.speed += asteroidSpeedIncrease * score;

                                    if (score % scoreForNextAsteroid == 0) {
                                        Asteroid newAsteroid;
                                        newAsteroid.x = 800 + rand() % 600;
                                        newAsteroid.y = rand() % 600;
                                        newAsteroid.speed = 2 + rand() % 4;
                                        asteroids.push_back(newAsteroid);
                                    }

                                    break;
                                }
                            }
                        }
                    }
                    ++it;
                }
            }

            std::string scoreStr = std::to_string(score);
            for(std::string::size_type i = 0; i < scoreStr.size(); i++) {
                SDL_Rect Message_rect; 
                Message_rect.x = 400 + i*16;
                Message_rect.y = 0; 
                Message_rect.w = 32;
                Message_rect.h = 32;
                SDL_RenderCopy(renderer, numberTexture[scoreStr[i] - '0'], NULL, &Message_rect);
            }

            for (auto it = explosions.begin(); it != explosions.end();) {
                if (currentTime - it->start_time > EXPLOSION_DURATION) {
                    it = explosions.erase(it);
                } else {
                    SDL_Rect explosionRect;
                    explosionRect.x = it->x;
                    explosionRect.y = it->y;
                    explosionRect.w = 128;
                    explosionRect.h = 128;
                    SDL_RenderCopy(renderer, explosionTexture, NULL, &explosionRect);
                    ++it;
                }
            }
        }

        SDL_RenderPresent(renderer);
    }

    for(int i = 0; i < 10; i++) {
    SDL_DestroyTexture(numberTexture[i]);
    }
    SDL_DestroyTexture(bgTexture);
    SDL_DestroyTexture(introTexture);
    SDL_DestroyTexture(menuTexture);
    SDL_DestroyTexture(deathTexture);
    SDL_DestroyTexture(playerTexture1);
    SDL_DestroyTexture(playerTexture2);
    SDL_DestroyTexture(playerTexture3);
    SDL_DestroyTexture(asteroidTexture);
    SDL_DestroyTexture(bulletTexture);
    SDL_DestroyTexture(explosionTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    Mix_FreeChunk(introSound);
    Mix_FreeChunk(shootSound);
    Mix_FreeChunk(explosionSound);
    Mix_CloseAudio();
    SDL_Quit();

    return 0;
}
