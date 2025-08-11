//
// Created by_snake GRHRehabTech on 2025-07-15.
//
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

// char string[255] = "";

// Macros variable (HEIGHT_SNAKE, WIDTH_SNAKE)
#define HEIGHT_SNAKE 240 - 1
#define WIDTH_SNAKE 320 - 1

// Array_snake to store the coordinates of snake
// tail (x_snake-ax_snakeis, y_snake-ax_snakeis)
int snakeTailx_snake[100], snakeTaily_snake[100];

// Variable to store the length of the
// snake's tail
int snakeTailLen;

// Score and signal flags
int gameover, key_snake, score_snake;

// Coordinates of snake's head and fruit
int x_snake, y_snake, fruitx_snake, fruity_snake;


void SnakeGame::create()
{
    GameScene::create();

    // Flag to signal the gameover
    gameover = 0;

    // Initial coordinates of the snake
    x_snake = WIDTH_SNAKE / 2;
    y_snake = HEIGHT_SNAKE / 2;

    // Initial coordinates of the fruit
    fruitx_snake = rand() % WIDTH_SNAKE;
    fruity_snake = rand() % HEIGHT_SNAKE;
    while (fruitx_snake == 0)
        fruitx_snake = rand() % WIDTH_SNAKE;

    while (fruity_snake == 0)
        fruity_snake = rand() % HEIGHT_SNAKE;

    // Score initialzed
    score_snake = 0;

    m_engine.bind_serial_input_event(
        'w',
        [&]
        {
            key_snake = 3;
        }
    );

    m_engine.bind_serial_input_event(
        'a',
        [&]
        {
            key_snake = 1;
        }
    );

    m_engine.bind_serial_input_event(
        's',
        [&]
        {
            key_snake = 4;
        }
    );

    m_engine.bind_serial_input_event(
        'd',
        [&]
        {
            key_snake = 2;
        }
    );

    m_engine.bind_serial_input_event(
        'x_snake',
        [&]
        {
            gameover = 1;
        }
    );

    m_engine.bind_serial_input_event(
        'f',
        [&]
        {
            x_snake = fruitx_snake;
            y_snake = fruity_snake;
        }
    );
}

void SnakeGame::render()
{
    GameScene::render();

    m_framebuffer.clear(0); // colors are based on pallette

    // Creating top wall
    // for (int i = 0; i < WIDTH_SNAKE; i++) {
    //     // printf("-");
    //     m_framebuffer.draw_pix_snakeel(i, 0, 66);
    // }
    // printf("\n");

    m_framebuffer.draw_outline_rectangle(0, 0, 320 - 1, 240 - 1, 45);
    // m_framebuffer.draw_pix_snakeel(x_snake, y_snake, 66);
    m_framebuffer.draw_filled_rectangle(x_snake, y_snake, 10, 10, 45);
    // m_framebuffer.draw_pix_snakeel(fruitx_snake, fruity_snake, 66);
    m_framebuffer.draw_filled_rectangle(fruitx_snake, fruity_snake, 5, 5, 31);


    for (int k = 0; k < snakeTailLen; k++)
    {
        m_framebuffer.draw_filled_rectangle(snakeTailx_snake[k], snakeTaily_snake[k], 10, 10, 45);
    }


    if (x_snake > 320) x_snake = 0;

    if (x_snake < 0) x_snake = 320;

    if (y_snake > 240) y_snake = 0;

    if (y_snake < 0) y_snake = 0;

    // Creating bottom walls with '-'
    // for (int i = 0; i < WIDTH_SNAKE; i++) {
    //     // printf("-");
    //     m_framebuffer.draw_pix_snakeel(i, 239, 27);
    // }
    // printf("\n");

    // Print the score and instructions
    // printf("score = %d\r\n", score);
    // printf("Press W, A, S, D for movement.\r\n");
    // printf("Press x_snake to quit the game.\r\n");
    struct mallinfo info = mallinfo();

    // sprintf(fps_string, "Total Heap: %d by_snaketes |  Free: %d \n", getTotalHeap(), getFreeHeap());
    // m_framebuffer.draw_string(fps_string, 5, 0, 100);
    //
    // sprintf(string, "Total Render Time %dms / %dms\n", m_engine.get_profile_render_loop_time(),
    //     m_engine.get_profile_main_loop_time());
    // m_framebuffer.draw_string(string, 5, 0, 110);
    //
    // sprintf(string, "Total Update: %dms / %dms\n",
    //     m_engine.get_profile_update_loop_time(), m_engine.get_profile_main_loop_time());
    // m_framebuffer.draw_string(string, 5, 0, 120);
    //
    // sprintf(string, "Total allocated: %d by_snaketes\n", info.uordblks);
    // m_framebuffer.draw_string(string, 5, 0, 60);
    // sprintf(string, "Total free: %d by_snaketes\n", info.fordblks);
    // m_framebuffer.draw_string(string, 5, 0, 70);
    // sprintf(string, "Total heap size: %d by_snaketes\n", info.arena);
    // m_framebuffer.draw_string(string, 5, 0, 80);
    // sprintf(string, "Largest free block: %d by_snaketes\n", info.ordblks);
    // m_framebuffer.draw_string(string, 5, 0, 90);
    // sprintf(string, "FPS: %d\r\n", m_engine.get_fps());
    // m_framebuffer.draw_string(string, 5, 0, 0);
    // sprintf(string, "score = %d\r\n", score);
    // m_framebuffer.draw_string(string, 5, 0, 10);
    // sprintf(string, "Press W, A, S, D for movement.\r\n");
    // m_framebuffer.draw_string(string, 5, 0, 20);
    // sprintf(string, "Press x_snake to quit the game.\r\n");
    // m_framebuffer.draw_string(string, 5, 0, 30);
    // sprintf(string, "Snake Coords: %d, %d\r\n", x_snake, y_snake);
    // m_framebuffer.draw_string(string, 5, 0, 40);
    // sprintf(string, "Food Coords: %d, %d\r\n", fruitx_snake, fruity_snake);
    // m_framebuffer.draw_string(string, 5, 0, 50);

    m_renderer.wait_for_vsync(); // If this is enabled debugging will not work, since this uses a blocking loop
    // with hardware event listeners it will just stay_snake here forever. Comment out the line when debugging

    m_framebuffer.swap_blocking();
}

void SnakeGame::update(double frameTime)
{
    GameScene::update(frameTime);
    // sprintf(string, "FPS: %d\r\n", m_engine.get_fps());
    // Updating the coordinates for continous
    // movement of snake
    int prevx_snake = snakeTailx_snake[0];
    int prevy_snake = snakeTaily_snake[0];
    int prev2x_snake, prev2y_snake;
    snakeTailx_snake[0] = x_snake;
    snakeTaily_snake[0] = y_snake;
    for (int i = 1; i < snakeTailLen; i++)
    {
        prev2x_snake = snakeTailx_snake[i];
        prev2y_snake = snakeTaily_snake[i];
        snakeTailx_snake[i] = prevx_snake;
        snakeTaily_snake[i] = prevy_snake;
        prevx_snake = prev2x_snake;
        prevy_snake = prev2y_snake;
    }

    // Changing the direction of movement of snake
    switch (key_snake)
    {
    case 1:
        x_snake--;
        break;
    case 2:
        x_snake++;
        break;
    case 3:
        y_snake--;
        break;
    case 4:
        y_snake++;
        break;
    default:
        break;
    }

    // If the game is over
    if (x_snake < 0 || x_snake >= WIDTH_SNAKE || y_snake < 0 || y_snake >= HEIGHT_SNAKE)
        gameover = 1;

    // Checks for collision with the tail (o)
    for (int i = 0; i < snakeTailLen; i++)
    {
        if (snakeTailx_snake[i] == x_snake && snakeTaily_snake[i] == y_snake)
            gameover = 1;
    }

    // If snake reaches the fruit
    // then update the score

    if (x_snake)

        if ((x_snake <= fruitx_snake && x_snake + 10 >= fruitx_snake) && (y_snake <= fruity_snake && y_snake + 10 >= fruity_snake))
        {
            fruitx_snake = rand() % WIDTH_SNAKE;
            fruity_snake = rand() % HEIGHT_SNAKE;
            while (fruitx_snake == 0)
                fruitx_snake = rand() % WIDTH_SNAKE;

            // Generation of new fruit
            while (fruity_snake == 0)
                fruity_snake = rand() % HEIGHT_SNAKE;
            score_snake += 10;
            snakeTailLen++;
        }
}

void SnakeGame::destroy()
{
    GameScene::destroy();
}
