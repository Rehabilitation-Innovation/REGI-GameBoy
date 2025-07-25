//
// Created by abina on 7/16/2025.
//

#include "DinoGame.h"
#include <cstdlib>
#include <format>

#include "tinyengine.h"
#include "tinyengine_framebuffer.h"
#include "tinyengine_sprite.h"
#define CUTE_C2_IMPLEMENTATION
#include "cute_c2.h"
#include "hardware/gpio.h"
#include <vector>
#include  "pico/rand.h"
#include "SnakeGame.h"

#include <malloc.h>

#include "cactus_sprite.h"
#include "dino_running.h"
#include "dino_sprite.h"
#include "TinyEngineUI.h"
// Global variables for jump height and game speed
int jumpHeight = 50, gameSpeed = 40;
int jump = 1;
uint32_t x = 0, y = 150;
float vely = 0.0f, gravity = 0.5f;

bool onGround = true, gameStarted = false;
std::string start_text = "PRESS JUMP TO START";
TinyEngineUIBlinkingTextBox banner(160 - (20 + start_text.length() * 7) / 2, 200, 20 + start_text.length() * 7, 20, 0,
    15, start_text, 5);

uint16_t jump_counter = 0, miss = 0;
/**
 * ------------------------------------------
 * Score:               Jumps:
 * Time:                Missed Jumps:
 * ------------------------------------------
 */
TinyEngineUIText score(5, 5, "Score: ", 15);
TinyEngineUIText time_(5, 30, "Time: ", 15);
TinyEngineUIText jumps(90, 5, "Jumps: ", 15);
TinyEngineUIText missed_jumps(90, 30, "Missed Jumps: ", 15);

TinyEngineUITextBox scoreboard(70, 180, 210, 50, 45);

DinoGame::~DinoGame()
{
}

Sprite dinoSprite, cactusSprite;
c2AABB dinoBox, cactusBox;


void DinoGame::create()
{
    GameScene::create();
    scoreboard.add_text("s", score);
    scoreboard.add_text("j", jumps);
    scoreboard.add_text("t", time_);
    scoreboard.add_text("mj", missed_jumps);
    dinoSprite = Sprite(
        te_sprite_t{
            .sprite_buffer = dino_data,
            .width = 24,
            .height = 24,
            .sprite_palette = 0
        },
        te_sprite_animation_t{
            .sprite_animation_frames = dino_frames,
            .animation_delay = 5,
            .total_frames = 11,
            .current_frame = 0,
            .start_frame = 3,
            .end_frame = 8
        });
    dinoSprite.set_m_animated(true);
    dinoSprite.set_m_x(100);
    dinoSprite.set_m_y(150);
    dinoBox.min = { .x = (float)dinoSprite.get_m_x(), .y = (float)dinoSprite.get_m_y() };
    dinoBox.max = {
        .x = (float)dinoSprite.get_m_x() + dinoSprite.get_m_sprite_data()->width,
        .y = (float)dinoSprite.get_m_y() + dinoSprite.get_m_sprite_data()->height
    };
    cactusSprite = Sprite(
        te_sprite_t{
            .sprite_buffer = cactus_data,
            .width = cactus_width,
            .height = cactus_height,
            .sprite_palette = 0
        });
    cactusSprite.set_m_x(240);
    cactusSprite.set_m_y(150);
    cactusBox.min = { .x = (float)cactusSprite.get_m_x(), .y = (float)cactusSprite.get_m_y() };
    cactusBox.max = {
        .x = (float)cactusSprite.get_m_x() + cactusSprite.get_m_sprite_data()->width,
        .y = (float)cactusSprite.get_m_y() + cactusSprite.get_m_sprite_data()->height
    };
    m_engine.bind_serial_input_event(
        'w',
        [&]
        {
            if (!gameStarted) gameStarted = true;
            if (true == onGround)
            {
                jump_counter++;
                vely = -10;
                onGround = false;
            }
        }
    );
    m_engine.bind_serial_input_event(
        'x',
        [&]
        {
            gameStarted = false;
        }
    );
}

char detail[255] = { 0 };

void DinoGame::render()
{
    m_framebuffer.clear(0);
    // m_framebuffer.draw_grid(10, 15);
    GameScene::render();

    m_framebuffer.draw_sprite(dinoSprite);
    m_framebuffer.draw_sprite(cactusSprite);

    // sprintf(detail, "vely: %f", vely);
    // m_framebuffer.draw_string(detail, 5, 70, 130);
    //
    // sprintf(detail, "Jump Input: %d", jump);
    // m_framebuffer.draw_string(detail, 5, 80, 140);

    // score.render(m_framebuffer);
    scoreboard.render(m_framebuffer);
    if (!gameStarted)
        banner.render(m_framebuffer);
    // m_renderer.wait_for_vsync();
    m_framebuffer.swap_blocking();
}

char temp[255] = { 0 };
double t = 0;
bool alreadyHit = false;

void DinoGame::update(double frameTime)
{
    // GameScene::update(frameTime);
    dinoSprite.set_m_frametime(dinoSprite.get_m_frametime() + frameTime * 100);
    vely += gravity;
    dinoSprite.set_m_y(dinoSprite.get_m_y() + vely);

    dinoBox.min = { .x = (float)dinoSprite.get_m_x(), .y = (float)dinoSprite.get_m_y() };
    dinoBox.max = {
        .x = (float)dinoSprite.get_m_x() + dinoSprite.get_m_sprite_data()->width,
        .y = (float)dinoSprite.get_m_y() + dinoSprite.get_m_sprite_data()->height
    };

    if (gameStarted)
        cactusSprite.set_m_x(cactusSprite.get_m_x() - frameTime * 100);

    cactusBox.min = { .x = (float)cactusSprite.get_m_x(), .y = (float)cactusSprite.get_m_y() };
    cactusBox.max = {
        .x = (float)cactusSprite.get_m_x() + cactusSprite.get_m_sprite_data()->width,
        .y = (float)cactusSprite.get_m_y() + cactusSprite.get_m_sprite_data()->height
    };

    if (cactusSprite.get_m_x() < 10)
    {
        alreadyHit = false;
        cactusSprite.set_m_x(310);
    }
    if (c2AABBtoAABB(dinoBox, cactusBox) && !alreadyHit)
    {
        alreadyHit = true;
        miss++;
    }

    if (dinoSprite.get_m_y() > 150)
    {
        onGround = true;
        dinoSprite.set_m_y(150);
        vely = 0.0f;
    }

    t += frameTime;
    sprintf(temp, "Score:%.2f", frameTime);
    score.set_text(temp);
    sprintf(temp, "Jumps:%d", jump_counter);
    jumps.set_text(temp);
    sprintf(temp, "Time:%1.0fs", t);
    time_.set_text(temp);
    sprintf(temp, "Missed Jumps:%d", miss);
    missed_jumps.set_text(temp);


    banner.update(frameTime);
}

void DinoGame::destroy()
{
    GameScene::destroy();
}
