//
// Created by abina on 7/16/2025.
//

#include "DinoGame.h"
#include <cstdlib>

#include "tinyengine.h"
#include "dino_running.h"
#include "tinyengine_framebuffer.h"
#include "tinyengine_sprite.h"

#include "hardware/gpio.h"
#include  "cactus.h"
#include <vector>
#include  "pico/rand.h"
#include "SnakeGame.h"

#include <malloc.h>

// Global variables for jump height and game speed
int jumpHeight, gameSpeed = 40;

DinoGame::~DinoGame()
{
}

void DinoGame::create()
{
    GameScene::create();
}

void DinoGame::render()
{
    GameScene::render();
}

void DinoGame::update(double frameTime)
{
    GameScene::update(frameTime);
}

void DinoGame::destroy()
{
    GameScene::destroy();
}
