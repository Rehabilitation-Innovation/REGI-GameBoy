// //
// // Created by abina on 7/16/2025.
// //

// #include "DinoGame.h"
// #include <cstdlib>
// #include <format>

// #include "tinyengine.h"
// #include "tinyengine_framebuffer.h"
// #include "tinyengine_sprite.h"
// #include "hardware/gpio.h"
// #include <vector>
// #include  "pico/rand.h"
// #include "SnakeGame.h"
// #include "cute_c2.h"
// #include <malloc.h>

// #include "cactus_sprite.h"
// #include "dino_running.h"
// #include "dino_sprite.h"
// #include "TinyEngineUI.h"

// #include "hardware/dma.h"
// #include "hardware/interp.h"

// #include "pico/stdlib.h"
// #include "hardware/pio.h"
// #include "hardware/clocks.h"
// #include "ws2812.pio.h"
// #include "hardware/adc.h"
// #include "hardware/dma.h"

// #include "pico/time.h"
// #include "pico_tone.hpp"
// #include "hardware/watchdog.h"

// #define IS_RGBW true
// #define NUM_PIXELS 4

// #ifdef PICO_DEFAULT_WS2812_PIN
// #define WS2812_PIN PICO_DEFAULT_WS2812_PIN
// #else
// // default to pin 2 if the board doesn't have a default WS2812 pin defined
// #define WS2812_PIN 6
// #endif

// // Check the pin is compatible with the platform
// #if WS2812_PIN >= NUM_BANK0_GPIOS
// #error Attempting to use a pin>=32 on a platform that does not support it
// #endif

// static inline void put_pixel(PIO pio, uint sm, uint32_t pixel_grb)
// {
//     pio_sm_put_blocking(pio, sm, pixel_grb << 8u);
// }

// static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
// {
//     return
//         ((uint32_t)(r) << 8) |
//         ((uint32_t)(g) << 16) |
//         (uint32_t)(b);
// }

// static inline uint32_t urgbw_u32(uint8_t r, uint8_t g, uint8_t b, uint8_t w)
// {
//     return
//         ((uint32_t)(r) << 8) |
//         ((uint32_t)(g) << 16) |
//         ((uint32_t)(w) << 24) |
//         (uint32_t)(b);
// }

// void pattern_snakes(PIO pio, uint sm, uint len, uint t)
// {
//     for (uint i = 0; i < len; ++i)
//     {
//         uint x = (i + (t >> 1)) % 64;
//         if (x < 10)
//             put_pixel(pio, sm, urgb_u32(0xff, 0, 0));
//         else if (x >= 15 && x < 25)
//             put_pixel(pio, sm, urgb_u32(0, 0xff, 0));
//         else if (x >= 30 && x < 40)
//             put_pixel(pio, sm, urgb_u32(0, 0, 0xff));
//         else
//             put_pixel(pio, sm, 0);
//     }
// }

// void pattern_random(PIO pio, uint sm, uint len, uint t)
// {
//     if (t % 8)
//         return;
//     for (uint i = 0; i < len; ++i)
//         put_pixel(pio, sm, urgbw_u32(rand() / 100000, rand() / 100000, rand() / 100000, rand() / 100000));
// }

// void pattern_sparkle(PIO pio, uint sm, uint len, uint t)
// {
//     if (t % 8)
//         return;
//     for (uint i = 0; i < len; ++i)
//         put_pixel(pio, sm, rand() % 16 ? 0 : 0xffffffff);
// }

// void pattern_greys(PIO pio, uint sm, uint len, uint t)
// {
//     uint max = 100; // let's not draw too much current!
//     t %= max;
//     for (uint i = 0; i < len; ++i)
//     {
//         put_pixel(pio, sm, t * 0x10101);
//         if (++t >= max) t = 0;
//     }
// }

// typedef void (*pattern)(PIO pio, uint sm, uint len, uint t);

// const struct
// {
//     pattern pat;
//     const char* name;
// } pattern_table[] = {
//     {pattern_snakes, "Snakes!"},
//     {pattern_random, "Random data"},
//     {pattern_sparkle, "Sparkles"},
//     {pattern_greys, "Greys"},
// };

// // Global variables for jump height and game speed
// int jumpHeight = 50, gameSpeed = 40;
// int jump = 1;
// uint32_t x = 0, y = 150;
// float vely = 0.0f, gravity = 0.5f;

// bool onGround = true, gameStarted = false;
// std::string start_text = "PRESS JUMP TO START";
// TinyEngineUIBlinkingTextBox banner(160 - (20 + start_text.length() * 7) / 2, 200, 20 + start_text.length() * 7, 20, 0,
//     15, start_text, 5);

// uint16_t jump_counter = 0, miss = 0;
// /**
//  * ------------------------------------------
//  * Score:               Jumps:
//  * Time:                Missed Jumps:
//  * ------------------------------------------
//  */
// TinyEngineUIText score(5, 5, "Score: ", 15);
// TinyEngineUIText time_(5, 30, "Time: ", 15);
// TinyEngineUIText jumps(90, 5, "Jumps: ", 15);
// TinyEngineUIText missed_jumps(90, 30, "Missed Jumps: ", 15);

// TinyEngineUITextBox scoreboard(70, 180, 210, 50, 45);

// TinyEngineUIText input(0, 20, "Input: ", 15);

// DinoGame::~DinoGame()
// {
// }

// Sprite dinoSprite, cactusSprite;
// c2AABB dinoBox, cactusBox;

// PIO pio;
// uint sm;
// uint offsetpio;
// Tone myPlayer(10, 50, 0, 20, 0, 20, 0, 10);
// int melody[] = { NOTE_C4, 4, NOTE_G3, 8, NOTE_G3, 8, NOTE_A3, 4, NOTE_G3, 4, 0, 4, NOTE_B3, 4, NOTE_C4, 4 };

// int play_tone = 0;

// void DinoGame::create()
// {
//     printf("Init Machine & dma channel!\n");
//     int res = myPlayer.init(TONE_NON_BLOCKING, true);
//     printf("Result of initializing tone = 0x%X\n", res);
//     adc_init();

//     // Make sure GPIO is high-impedance, no pullups etc
//     adc_gpio_init(44);
//     // Select ADC input 0 (GPIO26)
//     adc_select_input(4);


//     // todo get free sm


//     // This will find a free pio and state machine for our program and load it for us
//     // We use pio_claim_free_sm_and_add_program_for_gpio_range (for_gpio_range variant)
//     // so we will get a PIO instance suitable for addressing gpios >= 32 if needed and supported by the hardware
//     bool success = pio_claim_free_sm_and_add_program_for_gpio_range(&ws2812_program, &pio, &sm, &offsetpio, WS2812_PIN,
//         1,
//         true);
//     hard_assert(success);

//     ws2812_program_init(pio, sm, offsetpio, WS2812_PIN, 1000000, IS_RGBW);

//     int t = 0;
//     // while (1) {
//     //     int pat = rand() % count_of(pattern_table);
//     //     int dir = (rand() >> 30) & 1 ? 1 : -1;
//     //     puts(pattern_table[pat].name);
//     //     puts(dir == 1 ? "(forward)" : "(backward)");
//     //     for (int i = 0; i < 1000; ++i) {
//     //         pattern_table[pat].pat(pio, sm, NUM_PIXELS, t);
//     //         sleep_ms(10);
//     //         t += dir;
//     //     }
//     // }

//     // This will free resources and unload our program
//     pio_remove_program_and_unclaim_sm(&ws2812_program, pio, sm, offsetpio);

//     GameScene::create();
//     scoreboard.add_text("s", score);
//     scoreboard.add_text("j", jumps);
//     scoreboard.add_text("t", time_);
//     scoreboard.add_text("mj", missed_jumps);
//     dinoSprite = Sprite(
//         te_sprite_t{
//             .sprite_buffer = dino_data,
//             .width = 24,
//             .height = 24,
//             .sprite_palette = 0
//         },
//         te_sprite_animation_t{
//             .sprite_animation_frames = dino_frames,
//             .animation_delay = 5,
//             .total_frames = 11,
//             .current_frame = 0,
//             .start_frame = 3,
//             .end_frame = 8
//         });
//     dinoSprite.set_m_animated(true);
//     dinoSprite.set_m_x(100);
//     dinoSprite.set_m_y(150);
//     dinoBox.min = { .x = (float)dinoSprite.get_m_x(), .y = (float)dinoSprite.get_m_y() };
//     dinoBox.max = {
//         .x = (float)dinoSprite.get_m_x() + dinoSprite.get_m_sprite_data()->width,
//         .y = (float)dinoSprite.get_m_y() + dinoSprite.get_m_sprite_data()->height
//     };
//     cactusSprite = Sprite(
//         te_sprite_t{
//             .sprite_buffer = cactus_data,
//             .width = cactus_width,
//             .height = cactus_height,
//             .sprite_palette = 0
//         });
//     cactusSprite.set_m_x(240);
//     cactusSprite.set_m_y(150);
//     cactusBox.min = { .x = (float)cactusSprite.get_m_x(), .y = (float)cactusSprite.get_m_y() };
//     cactusBox.max = {
//         .x = (float)cactusSprite.get_m_x() + cactusSprite.get_m_sprite_data()->width,
//         .y = (float)cactusSprite.get_m_y() + cactusSprite.get_m_sprite_data()->height
//     };
//     m_engine.bind_serial_input_event(
//         'w',
//         [&]
//         {
//             if (!gameStarted)
//             {
//                 // memset(m_framebuffer.pixel_buffer_back, 0, m_framebuffer.m_pixel_buffer_size);
//                 // m_framebuffer.swap_blocking();
//                 gameStarted = true;
//             }
//             if (true == onGround)
//             {
//                 jump_counter++;
//                 vely = -10;
//                 onGround = false;
//             }
//             play_tone = 1;
//             // myPlayer.tone(15000,0.25);
//             // int pat = rand() % count_of(pattern_table);
//             // int dir = (rand() >> 30) & 1 ? 1 : -1;
//             // // puts(pattern_table[pat].name);
//             // // puts(dir == 1 ? "(forward)" : "(backward)");
//             // // for (int i = 0; i < 1000; ++i) {
//             // pattern_table[pat].pat(pio, sm, NUM_PIXELS, t);
//             // // sleep_ms(10);
//             // // t += dir;
//             // // }

//             // put_pixel(pio, sm, urgb_u32(0xff, 0, 0));
//             // put_pixel(pio, sm, urgb_u32(0, 0xff, 0));
//             // put_pixel(pio, sm, urgb_u32(0, 0, 0xff));
//         }
//     );
//     m_engine.bind_serial_input_event(
//         'x',
//         [&]
//         {
//             gameStarted = false;
//         }
//     );
//     m_engine.bind_serial_input_event(
//         'r',
//         [&]
//         {
//             watchdog_reboot(0, 0, 10);
//             // gameStarted = false;
//         }
//     );
//     m_framebuffer.clear(0);
//     m_framebuffer.swap_blocking();
//     m_framebuffer.clear(0);
// }

// // char fps_frame_buf[8 * 9 * 10] = { 0 };

// static void blit_rect_dma(uint8_t* framebuffer)
// {
//     // int dma_chan = dma_claim_unused_channel(true);
//     // dma_channel_config cfg = dma_channel_get_default_config(dma_chan);
//     // channel_config_set_transfer_data_size(&cfg, DMA_SIZE_8);
//     // channel_config_set_read_increment(&cfg, true);
//     // channel_config_set_write_increment(&cfg, true);

//     // Set up interpolator to compute row base addresses
//     interp_config icfg = interp_default_config();
//     interp_config_set_add_raw(&icfg, true);
//     interp_config_set_shift(&icfg, 0);
//     interp_config_set_mask(&icfg, 0, 31); // full 32-bit mask
//     interp_set_config(interp0, 0, &icfg);


//     int src_index = 0;

//     for (int row = 0; row < 100; row++)
//     {
//         int row_start_index = (50 + row) * 320 + 50;

//         interp0->base[0] = row_start_index;
//         interp0->accum[0] = 0;

//         for (int col = 0; col < 100; col++)
//         {
//             // uint32_t framebuffer_index = interp_pop_full_result(interp0, 0);
//             printf("%d\r\n", interp_pop_full_result(interp0));
//             // framebuffer[interp0->pop[0]] = 15; //source_region[src_index++];
//             interp0->accum[0] += 1;
//         }
//     }
// }


// void DinoGame::render()
// {
//     m_framebuffer.clear(0);
//     // m_framebuffer.draw_filled_rectangle(0, 0, 100, 30, 0);
//     // m_framebuffer.draw_filled_rectangle(0, 120, 320, 240-120, 0);
//     // m_framebuffer.draw_grid(10, 15);
//     GameScene::render();
//     // m_framebuffer.draw_filled_rectangle(dinoSprite.get_m_x(), dinoSprite.get_m_y(),
//     //                                     dinoSprite.get_m_sprite_data()->width, dinoSprite.get_m_sprite_data()->height,
//     //                                     0);
//     m_framebuffer.draw_sprite(dinoSprite);
//     // m_framebuffer.draw_filled_rectangle(cactusSprite.get_m_x(), cactusSprite.get_m_y(),
//     //                                     cactusSprite.get_m_sprite_data()->width,
//     //                                     cactusSprite.get_m_sprite_data()->height,
//     //                                     0);
//     m_framebuffer.draw_sprite(cactusSprite);
//     // input.render(m_framebuffer);
//     //
//     // // sprintf(detail, "vely: %f", vely);
//     // // m_framebuffer.draw_string(detail, 5, 70, 130);
//     // //
//     // // sprintf(detail, "Jump Input: %d", jump);
//     // // m_framebuffer.draw_string(detail, 5, 80, 140);
//     //
//     // blit_rect_dma(m_framebuffer.pixel_buffer_back);
//     // m_framebuffer.draw_filled_rectangle(50, 50, 100, 100, 15);
//     // // score.render(m_framebuffer);
//     // m_framebuffer.draw_filled_rectangle(scoreboard.get_m_x(), scoreboard.get_m_y(),
//     //                                     scoreboard.get_m_width(), scoreboard.get_m_height(),
//     //                                     0);
//     scoreboard.render(m_framebuffer);
//     if (!gameStarted)
//     {
//         // m_framebuffer.draw_filled_rectangle(banner.get_m_x(), banner.get_m_y(),
//         //                                     banner.get_m_width(), banner.get_m_height(),
//         //                                     0);
//         banner.render(m_framebuffer);
//     }
//     // m_renderer.wait_for_vsync();
//     m_framebuffer.swap_blocking();
// }

// char temp[255] = { 0 };
// double t = 0;
// bool alreadyHit = false;

// void DinoGame::update(double frameTime) {
//     GameScene::update(frameTime);
//     dinoSprite.set_m_frametime(dinoSprite.get_m_frametime() + frameTime * 100);
//     vely += gravity;

//     // put_pixel(pio, sm, urgb_u32(0xff - vely, 0, 0));
//     // put_pixel(pio, sm, urgb_u32(0, 0xff - vely, 0));
//     // put_pixel(pio, sm, urgb_u32(0, 0, 0xff - vely));

//     dinoSprite.set_m_y(dinoSprite.get_m_y() + vely);

//     dinoBox.min = { .x = (float)dinoSprite.get_m_x(), .y = (float)dinoSprite.get_m_y() };
//     dinoBox.max = {
//         .x = (float)dinoSprite.get_m_x() + dinoSprite.get_m_sprite_data()->width,
//         .y = (float)dinoSprite.get_m_y() + dinoSprite.get_m_sprite_data()->height
//     };

//     if (gameStarted)
//         cactusSprite.set_m_x(cactusSprite.get_m_x() - frameTime * 100);

//     cactusBox.min = { .x = (float)cactusSprite.get_m_x(), .y = (float)cactusSprite.get_m_y() };
//     cactusBox.max = {
//         .x = (float)cactusSprite.get_m_x() + cactusSprite.get_m_sprite_data()->width,
//         .y = (float)cactusSprite.get_m_y() + cactusSprite.get_m_sprite_data()->height
//     };

//     if (cactusSprite.get_m_x() < 10)
//     {
//         alreadyHit = false;
//         cactusSprite.set_m_x(310);
//     }
//     if (c2AABBtoAABB(dinoBox, cactusBox) && !alreadyHit)
//     {
//         alreadyHit = true;
//         miss++;
//     }

//     if (dinoSprite.get_m_y() > 150)
//     {
//         onGround = true;
//         dinoSprite.set_m_y(150);
//         vely = 0.0f;
//     }

//     t += frameTime;
//     sprintf(temp, "Score:%.2f", frameTime);
//     score.set_text(temp);
//     sprintf(temp, "Jumps:%d", jump_counter);
//     jumps.set_text(temp);
//     sprintf(temp, "Time:%1.0fs", t);
//     time_.set_text(temp);
//     sprintf(temp, "Missed Jumps:%d", miss);
//     missed_jumps.set_text(temp);

//     // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
//     const float conversion_factor = 3.3f / (1 << 12);
//     uint16_t result = adc_read();
//     // printf("Raw value: 0x%03x, voltage: %f V\n", result, result * conversion_factor);
//     // sleep_ms(500);
//     sprintf(temp, "Input: %f", result * conversion_factor);
//     input.set_text(temp);
//     banner.update(frameTime);

//     static double tone_time = 0;

//     // if (play_tone && tone_time <= 10) {
//     //     // myPlayer.play_melody(100, 3, new int[]{NOTE_A1, NOTE_D1, NOTE_A2});
//     //     myPlayer.tone(10000 + frameTime * 100, 0.01);
//     //     tone_time += frameTime * 100;
//     // }
//     //
//     // if (tone_time > 10) {
//     //     tone_time = 0;
//     //     play_tone = 0;
//     // }

//     int pat = rand() % count_of(pattern_table);
//     int dir = (rand() >> 30) & 1 ? 1 : -1;
//     // puts(pattern_table[pat].name);
//     // puts(dir == 1 ? "(forward)" : "(backward)");
//     // for (int i = 0; i < 1000; ++i) {
//     // static double rgb_time = 0;
//     // // rgb_time += frameTime ;
//     // if (rgb_time > 1)
//     // {
//     pattern_table[1].pat(pio, sm, NUM_PIXELS, frameTime);
//     //     rgb_time = 0;
//     // }
//     // rgb_time += frameTime * 10;
//     // sleep_ms(10);
//     // t += dir;
//     // }
// }

// void DinoGame::destroy()
// {
//     GameScene::destroy();
// }
