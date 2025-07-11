#include "main.h"
#include "game_scene.h"

#include "dino_running.h"
#include "tinyengine_framebuffer.h"
#include "tinyengine_sprite.h"
#include "tinyengine_sprite_animation.h"
#include "hardware/gpio.h"
#include "pico/rand.h"
#include  "cactus.h"

Sprite* dino, * cactus_sprite;
te_sprite_t dino_data;
te_sprite_animation_t dino_ani_data;

te_sprite_t cactus_data;

struct {
    double x = 0;
    double y = 150;
} obstacle;

double x = 0, y = 150;
double dx = 0, dy = 0;

void GameScene::create() {
    gpio_init(7);
    gpio_set_dir(7, GPIO_IN);
    gpio_pull_up(7);

    dino_ani_data = {
        .sprite_animation_frames = dino_frames,
        .width = 24,
        .height = 24,
        .animation_delay = 1,
        .total_frames = 11,
        .current_frame = 0,
        .current_frame_time = 0,
        .start_frame = 5,
        .end_frame = 11
    };

    dino_data = {
        .sprite_buffer = 0,
        .width = 24,
        .height = 24,
        .scale_x = 1,
        .scale_y = 1,
        .rotation = 0,
        .sprite_palette = 0
    };

    dino = new Sprite(&dino_data, &dino_ani_data);
    dino->set_x(90);

    cactus_data = {
    .sprite_buffer = cactus,
    .width = 64,
    .height = 64,
    .scale_x = 1,
    .scale_y = 1,
    .rotation = 0,
    .sprite_palette = 0
    };
    cactus_sprite = new Sprite(&cactus_data);
    cactus_sprite->set_x(150);
}

void GameScene::destroy() {
};

int jump = 0;

void GameScene::render() {
    m_framebuffer.clear(0); // colors are based on pallette

    // m_framebuffer.draw_filled_rectangle(x, y, 100, 100, (jump) ? 0xC0 : 0xF0);
    // te_sprite_render_animation(&m_framebuffer, &dino, 100, 100, 5);
    cactus_sprite->render(m_framebuffer);
    dino->render(m_framebuffer);

    for (int i = 0; i < 255; ++i) {
        m_framebuffer.draw_pixel(i, 0, i);
    }

    m_renderer.wait_for_vsync();
    m_framebuffer.swap_blocking();
};

void GameScene::update(double frameTime) {
    // telog("Update");
    // x += 100 * frameTime;

    // if (x >= 320) x = 0;
    obstacle.x -= 100 * frameTime;
    if (obstacle.x < 0) obstacle.x = 320;
    // y += dy * frameTime;
    //
    // dy *= frameTime;

    // if (y >=  150) dy = -2;

    // y += 100 * frameTime;

    if (!gpio_get(7)) {
        jump = 1;
        // printf("pused\r\n");
        y -= 200 * frameTime;
        // telog("Pressed");
    }
    else
        // if (y < 145)
        y += 140 * frameTime;

    if (y > 150) {
        y = 150;
    }

    // if (y < 0)
    //     y = 150;
    printf("%f %f\r\n", x, y);


    // else {
    //     jump = 0;
    // }

    // if (jump) {
    //     dx = 0;
    //     dy = 2;
    //     jump = 0;
    // }

    // if (y >= 240) y = 150;
    // if (y <= 10) y = 150;
    // dino->set_x(x);
    dino->set_y(y);
    dino_ani_data.current_frame_time += frameTime * 10;

    cactus_sprite->set_x(obstacle.x);
    cactus_sprite->set_y(obstacle.y);
};


