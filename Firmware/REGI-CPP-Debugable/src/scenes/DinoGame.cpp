//
// Created by abina on 7/16/2025.
//

#include "DinoGame.h"
#include <cstdlib>

#include "tinyengine.h"
#include "tinyengine_framebuffer.h"
#include "tinyengine_sprite.h"

#include "hardware/gpio.h"
#include <vector>
#include  "pico/rand.h"
#include "SnakeGame.h"

#include <malloc.h>

#include "cactus_sprite.h"
#include "dino_running.h"
#include "dino_sprite.h"

// Global variables for jump height and game speed
int jumpHeight = 50, gameSpeed = 40;
int jump = 1;
uint32_t x = 0, y = 150;
float vely = 0.0f, gravity = 0.5f;

bool onGround = true;

DinoGame::~DinoGame() {
}

Sprite dinoSprite, cactusSprite;

void DinoGame::create() {
    GameScene::create();

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
            .start_frame = 0,
            .end_frame = 10
        });
    dinoSprite.set_m_animated(true);
    dinoSprite.set_m_x(100);
    dinoSprite.set_m_y(150);

    cactusSprite = Sprite(
        te_sprite_t{
            .sprite_buffer = cactus_data,
            .width = cactus_width,
            .height = cactus_height,
            .sprite_palette = 0
        });

    m_engine.bind_serial_input_event(
        'w',
        [&] {
            if (true == onGround) {
                vely = -10;
                onGround = false;
            }
        }
    );
}

char detail[255] = {0};

void DinoGame::render() {
    m_framebuffer.clear(0);

    GameScene::render();

    m_framebuffer.draw_sprite(dinoSprite);
    m_framebuffer.draw_sprite(cactusSprite);

    sprintf(detail, "vely: %f", vely);
    m_framebuffer.draw_string(detail, 10, 0, 130);

    sprintf(detail, "Jump Input: %d", jump);
    m_framebuffer.draw_string(detail, 10, 0, 140);

    // m_renderer.wait_for_vsync();
    m_framebuffer.swap_blocking();
}

void DinoGame::update(double frameTime) {
    GameScene::update(frameTime);
    dinoSprite.set_m_frametime(dinoSprite.get_m_frametime() + frameTime * 100);
    vely += gravity;
    dinoSprite.set_m_y(dinoSprite.get_m_y() + vely);

    if (dinoSprite.get_m_y() > 150) {
        onGround = true;
        dinoSprite.set_m_y(150);
        vely = 0.0f;
    }

}

void DinoGame::destroy() {
    GameScene::destroy();
}
