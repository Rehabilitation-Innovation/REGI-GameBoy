// //
// // Created by GRHRehabTech on 2025-07-15.
// //
// #include <cstdlib>
//
// #include "tinyengine.h"
// #include "dino_running.h"
// #include "tinyengine_framebuffer.h"
// #include "tinyengine_sprite.h"
//
// #include "hardware/gpio.h"
// #include  "cactus.h"
// #include <vector>
// #include  "pico/rand.h"
// #include "SnakeGame.h"
//
// #include <malloc.h>
//
// // char string[255] = "";
//
// // Macros variable (HEIGHT, WIDTH)
// #define HEIGHT 240 - 1
// #define WIDTH 320 - 1
//
// // Array to store the coordinates of snake
// // tail (x-axis, y-axis)
// int snakeTailX[100], snakeTailY[100];
//
// // Variable to store the length of the
// // snake's tail
// int snakeTailLen;
//
// // Score and signal flags
// int gameover, key, score;
//
// // Coordinates of snake's head and fruit
// int x, y, fruitx, fruity;
//
//
//
//
// void SnakeGame::create() {
//     GameScene::create();
//
//     // Flag to signal the gameover
//     gameover = 0;
//
//     // Initial coordinates of the snake
//     x = WIDTH / 2;
//     y = HEIGHT / 2;
//
//     // Initial coordinates of the fruit
//     fruitx = rand() % WIDTH;
//     fruity = rand() % HEIGHT;
//     while (fruitx == 0)
//         fruitx = rand() % WIDTH;
//
//     while (fruity == 0)
//         fruity = rand() % HEIGHT;
//
//     // Score initialzed
//     score = 0;
//
//     m_engine.bind_serial_input_event(
//         'w',
//         [&] {
//             key = 3;
//         }
//     );
//
//     m_engine.bind_serial_input_event(
//         'a',
//         [&] {
//             key = 1;
//         }
//     );
//
//     m_engine.bind_serial_input_event(
//         's',
//         [&] {
//             key = 4;
//         }
//     );
//
//     m_engine.bind_serial_input_event(
//         'd',
//         [&] {
//             key = 2;
//         }
//     );
//
//     m_engine.bind_serial_input_event(
//         'x',
//         [&] {
//             gameover = 1;
//         }
//     );
//
//     m_engine.bind_serial_input_event(
//         'f',
//         [&] {
//             x = fruitx;
//             y = fruity;
//         }
//     );
// }
//
// void SnakeGame::render() {
//     GameScene::render();
//
//     m_framebuffer.clear(0); // colors are based on pallette
//
//     // Creating top wall
//     // for (int i = 0; i < WIDTH; i++) {
//     //     // printf("-");
//     //     m_framebuffer.draw_pixel(i, 0, 66);
//     // }
//     // printf("\n");
//
//     m_framebuffer.draw_outline_rectangle(0, 0, 320 - 1, 240 - 1, 45);
//     // m_framebuffer.draw_pixel(x, y, 66);
//     m_framebuffer.draw_filled_rectangle(x, y, 10, 10, 45);
//     // m_framebuffer.draw_pixel(fruitx, fruity, 66);
//     m_framebuffer.draw_filled_rectangle(fruitx, fruity, 5, 5, 31);
//
//
//     for (int k = 0; k < snakeTailLen; k++) {
//         m_framebuffer.draw_filled_rectangle(snakeTailX[k], snakeTailY[k], 10, 10, 45);
//     }
//
//
//     if (x > 320) x = 0;
//
//     if (x < 0) x = 320;
//
//     if (y > 240) y = 0;
//
//     if (y < 0) y = 0;
//
//     // Creating bottom walls with '-'
//     // for (int i = 0; i < WIDTH; i++) {
//     //     // printf("-");
//     //     m_framebuffer.draw_pixel(i, 239, 27);
//     // }
//     // printf("\n");
//
//     // Print the score and instructions
//     // printf("score = %d\r\n", score);
//     // printf("Press W, A, S, D for movement.\r\n");
//     // printf("Press X to quit the game.\r\n");
//     struct mallinfo info = mallinfo();
//
//     // sprintf(fps_string, "Total Heap: %d bytes |  Free: %d \n", getTotalHeap(), getFreeHeap());
//     // m_framebuffer.draw_string(fps_string, 5, 0, 100);
//     //
//     // sprintf(string, "Total Render Time %dms / %dms\n", m_engine.get_profile_render_loop_time(),
//     //     m_engine.get_profile_main_loop_time());
//     // m_framebuffer.draw_string(string, 5, 0, 110);
//     //
//     // sprintf(string, "Total Update: %dms / %dms\n",
//     //     m_engine.get_profile_update_loop_time(), m_engine.get_profile_main_loop_time());
//     // m_framebuffer.draw_string(string, 5, 0, 120);
//     //
//     // sprintf(string, "Total allocated: %d bytes\n", info.uordblks);
//     // m_framebuffer.draw_string(string, 5, 0, 60);
//     // sprintf(string, "Total free: %d bytes\n", info.fordblks);
//     // m_framebuffer.draw_string(string, 5, 0, 70);
//     // sprintf(string, "Total heap size: %d bytes\n", info.arena);
//     // m_framebuffer.draw_string(string, 5, 0, 80);
//     // sprintf(string, "Largest free block: %d bytes\n", info.ordblks);
//     // m_framebuffer.draw_string(string, 5, 0, 90);
//     // sprintf(string, "FPS: %d\r\n", m_engine.get_fps());
//     // m_framebuffer.draw_string(string, 5, 0, 0);
//     // sprintf(string, "score = %d\r\n", score);
//     // m_framebuffer.draw_string(string, 5, 0, 10);
//     // sprintf(string, "Press W, A, S, D for movement.\r\n");
//     // m_framebuffer.draw_string(string, 5, 0, 20);
//     // sprintf(string, "Press X to quit the game.\r\n");
//     // m_framebuffer.draw_string(string, 5, 0, 30);
//     // sprintf(string, "Snake Coords: %d, %d\r\n", x, y);
//     // m_framebuffer.draw_string(string, 5, 0, 40);
//     // sprintf(string, "Food Coords: %d, %d\r\n", fruitx, fruity);
//     // m_framebuffer.draw_string(string, 5, 0, 50);
//
//     m_renderer.wait_for_vsync(); // If this is enabled debugging will not work, since this uses a blocking loop
//     // with hardware event listeners it will just stay here forever. Comment out the line when debugging
//
//     m_framebuffer.swap_blocking();
// }
//
// void SnakeGame::update(double frameTime) {
//     GameScene::update(frameTime);
//     // sprintf(string, "FPS: %d\r\n", m_engine.get_fps());
//     // Updating the coordinates for continous
//     // movement of snake
//     int prevX = snakeTailX[0];
//     int prevY = snakeTailY[0];
//     int prev2X, prev2Y;
//     snakeTailX[0] = x;
//     snakeTailY[0] = y;
//     for (int i = 1; i < snakeTailLen; i++) {
//         prev2X = snakeTailX[i];
//         prev2Y = snakeTailY[i];
//         snakeTailX[i] = prevX;
//         snakeTailY[i] = prevY;
//         prevX = prev2X;
//         prevY = prev2Y;
//     }
//
//     // Changing the direction of movement of snake
//     switch (key) {
//     case 1:
//         x--;
//         break;
//     case 2:
//         x++;
//         break;
//     case 3:
//         y--;
//         break;
//     case 4:
//         y++;
//         break;
//     default:
//         break;
//     }
//
//     // If the game is over
//     if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
//         gameover = 1;
//
//     // Checks for collision with the tail (o)
//     for (int i = 0; i < snakeTailLen; i++) {
//         if (snakeTailX[i] == x && snakeTailY[i] == y)
//             gameover = 1;
//     }
//
//     // If snake reaches the fruit
//     // then update the score
//
//     if (x)
//
//         if ((x <= fruitx && x + 10 >= fruitx) && (y <= fruity && y + 10 >= fruity)) {
//             fruitx = rand() % WIDTH;
//             fruity = rand() % HEIGHT;
//             while (fruitx == 0)
//                 fruitx = rand() % WIDTH;
//
//             // Generation of new fruit
//             while (fruity == 0)
//                 fruity = rand() % HEIGHT;
//             score += 10;
//             snakeTailLen++;
//         }
// }
//
// void SnakeGame::destroy() {
//     GameScene::destroy();
// }
