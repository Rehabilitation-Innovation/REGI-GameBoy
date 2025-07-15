#include "main.h"
#include "game_scene.h"

#include <cstdlib>

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
#include  "pico/rand.h"
Sprite dino, cactus_sprite;

struct
{
    double x = 0;
    double y = 150;
} obstacle;

double x = 0, y = 150;
double dx = 0, dy = 0;
int jump = 0;

int up = 0;
int down = 0;
int left = 0;
int right = 0;

void GameScene::create()
{


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

    cactus_sprite = Sprite(new te_sprite_t{
        .sprite_buffer = cactus,
        .width = 37,
        .height = 41,
        .sprite_palette = 0
        });
    cactus_sprite.set_m_x(150);
    cactus_sprite.set_m_y(150);


    m_engine.bind_serial_input_event(
        'w',
        [&]
        {
            up = 1;
        }
    );

    m_engine.bind_serial_input_event(
        'a',
        [&]
        {
            left = 1;
        }
    );


    m_engine.bind_serial_input_event(
        's',
        [&]
        {
            jump = 1;
        }
    );


    m_engine.bind_serial_input_event(
        'd',
        [&]
        {
            jump = 1;
        }
    );

    m_engine.bind_gpio_input_event(
        7,
        [&]
        {
            jump = 1;
        }
    );
}

void GameScene::destroy()
{
};

double x_loc = 160, y_loc = 120;

int up_down = 0;
int apple = 0;
double apple_x = rand() % 256;
double apple_y = rand() % 256;
int width = 10;
int height = 2;

int score = 0;

class Snake {
public:
    int width = 10;
    int height = 2;
    double x_loc = 160;
    double y_loc = 120;

    Snake(int width, int height, double x_loc, double y_loc)
        : width(width),
          height(height),
          x_loc(x_loc),
          y_loc(y_loc)
           {
        create();
    }

    uint32_t x_body[30] = {0};
    uint32_t y_body[30] = {0};





    void create() {
        int temp = 0;
        for (int i = 0; i< width; i++) {
            for (int j = 0; j < height; j++) {
                x_body[temp] = x_loc + i;
                y_body[temp] = y_loc + j;
                temp++;
            }
        }


    }

};


Snake snake = Snake(10, 2, 160, 120);

char fps_string[30] = "";

void GameScene::render()
{
    m_framebuffer.clear(0); // colors are based on pallette

    // for (int i = 0; i < snake.width * snake.height; i++) {
    //
    //     m_framebuffer.draw_pixel(snake.x_body[i], snake.y_body[i], 70);
    //
    // }
    // printf("Hello World!\n\r");
    // m_framebuffer.draw_pixel(apple_x, apple_y, 140);
    // m_framebuffer.draw_char('a', 0, 0);
    sprintf(fps_string, "FPS: %d", m_engine.get_fps());
    m_framebuffer.draw_string(fps_string, 5, 100, 100);




    m_renderer.wait_for_vsync(); // If this is enabled debugging will not work, since this uses a blocking loop
    // with hardware event listeners it will just stay here forever. Comment out the line when debugging

    m_framebuffer.swap_blocking();
};

uint8_t in;

void GameScene::update(double frameTime)
{
    if (jump == 1) {
        up_down = !up_down;
        jump = 0;
    }

    if (up_down) {
        if (snake.x_loc < 320) {
            snake.x_loc +=frameTime*100;
        }
        else {
            snake.x_loc = 0;
        }
    }
    else {
        if (snake.y_loc < 240) {
            snake.y_loc += frameTime*100;
        }
        else {
            snake.y_loc = 0;
        }
    }

    snake.create();



    if (snake.x_loc <= apple_x && apple_x <= snake.x_loc + snake.width) {
        if (snake.y_loc <= apple_y  && apple_y <= snake.y_loc + snake.height ) {
            score++;
            apple_x = rand() % 256;
            apple_y = rand() % 256;
            while (apple_y > 240) {
                apple_y = rand() % 256;
            }
            snake.width++;

        }

    }




};
