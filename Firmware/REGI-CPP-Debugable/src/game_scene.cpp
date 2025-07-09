#include "game_scene.h"
#include "tinyengine_framebuffer.h"
#include "main.h"
#include "hardware/gpio.h"
extern "C" {
#include "tinyengine_sprite.h"
#include "tinyengine_sprite_animation.h"
}
#include "dino_running.h"
te_sprite_animation_t dino;

double x = 0, y = 0;


// void gpio_callback(uint gpio, uint32_t events) {
//     // Put the GPIO event(s) that just happened into event_str
//     // so we can print it
//     gpio_event_string(event_str, events);
//     printf("GPIO %d %s\n", gpio, event_str);
// }

void GameScene::create() {

    gpio_set_dir(7, GPIO_IN);
    gpio_pull_up(7);

    dino = {
        .sprite_animation_frames = dino_frames,
        .width = 24,
        .height = 24,
        .animation_delay = 5,
        .total_frames = 10,
        .current_frame = 0,
        .current_frame_time = 0,
    };

};

void GameScene::destroy() {};

int jump = 0;

void GameScene::render() {

    m_framebuffer.clear(0x00);

    m_framebuffer.draw_filled_rectangle(x, y, 100, 100, (jump) ? 0xC0 : 0xF0);
    te_sprite_render_animation(&m_framebuffer, &dino, 100, 100, 5);
    m_renderer.wait_for_vsync();
    m_framebuffer.swap_blocking();

};

void GameScene::update(double frameTime) {
    // telog("Update");
    x += 100 * frameTime;

    if (x >= 320) x = 0;

    y += 100 * frameTime;

    if (gpio_get(7)) {
        jump = 1;
    }
    // else {
    //     jump = 0;
    // }

    if (y >= 240) y = 0;

};


