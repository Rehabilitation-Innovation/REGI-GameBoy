#include "main.h"
#include "game_scene.h"
#include "tinyengine.h"
#include "dino_running.h"
#include "tinyengine_framebuffer.h"
#include "tinyengine_sprite.h"

#include "hardware/gpio.h"
#include "pico/rand.h"
#include  "cactus.h"
#include "hardware/uart.h"
#include "hardware/irq.h"

#include <vector>

Sprite dino, cactus_sprite;
te_sprite_t dino_data;
te_sprite_animation_t dino_ani_data;

te_sprite_t cactus_data;


struct
{
    double x = 0;
    double y = 150;
} obstacle;

double x = 0, y = 150;
double dx = 0, dy = 0;
int jump = 0;

void GameScene::create()
{
    gpio_init(7);
    gpio_set_dir(7, GPIO_IN);
    gpio_pull_up(7);


    dino = Sprite(
        new te_sprite_t{
            .sprite_buffer = dino_frames[0],
            .width = 24,
            .height = 24,
            .sprite_palette = 0
        },
        new te_sprite_animation_t{
            .sprite_animation_frames = dino_frames,
            .animation_delay = 10,
            .total_frames = 11,
            .current_frame = 5,
            .start_frame = 3,
            .end_frame = 8
        });
    dino.set_m_animated(true);
    dino.set_m_x(90);

    cactus_data = {
        .sprite_buffer = cactus,
        .width = 37,
        .height = 41,
        .sprite_palette = 0
    };
    cactus_sprite = Sprite(new te_sprite_t{
        .sprite_buffer = cactus,
        .width = 37,
        .height = 41,
        .sprite_palette = 0
        });
    cactus_sprite.set_m_x(150);
    cactus_sprite.set_m_y(150);

    m_engine.bind_serial_input_event(
        'a',
        [&]
        {
            jump = 1;
        }
    );
}

void GameScene::destroy()
{
};

void GameScene::render()
{
    m_framebuffer.clear(0); // colors are based on pallette

    // m_framebuffer.draw_filled_rectangle(x, y, 100, 100, (jump) ? 0xC0 : 0xF0);
    // te_sprite_render_animation(&m_framebuffer, &dino, 100, 100, 5);
    // cactus_sprite->render(m_framebuffer);
    // dino->render(m_framebuffer);
    m_framebuffer.draw_sprite(cactus_sprite);
    m_framebuffer.draw_sprite(dino);

    for (int i = 0; i < 255; ++i)
    {
        m_framebuffer.draw_pixel(i, 0, i);
        m_framebuffer.draw_filled_rectangle(i + 20, i % 16 + 20, 5, 5, i);
    }

    m_renderer.wait_for_vsync(); // If this is enabled debugging will not work, since this uses a blocking loop
    // with hardware event listeners it will just stay here forever. Comment out the line when debugging

    m_framebuffer.swap_blocking();
};

uint8_t in;

void GameScene::update(double frameTime)
{
    obstacle.x -= 100 * frameTime;
    if (obstacle.x < 0) obstacle.x = 320;

    // if (!gpio_get(7))
    // {
    //     y -= 200 * frameTime;
    // }
    // else
    //     y += 140 * frameTime;

    if (jump)
    {
        y -= 200 * frameTime;
    }

    if (y <= 50) { jump = 0; }
    if (y < 150 && !jump) { y += 200 * frameTime; }
    if (y > 150)
    {
        y = 150;
    }
    dino.set_m_y(y);
    dino.set_m_frametime(dino.get_m_frametime() + frameTime);

    cactus_sprite.set_m_x(obstacle.x);
    // cactus_sprite.set_m_y(obstacle.y);
};
