#include <stdio.h>
#include <math.h>

#include "game.h"
#include "pico/stdlib.h"
#include "pico/rand.h"
#include "hardware/pwm.h"

#include "tinyengine.h"
#include "tinyengine_renderer.h"
#include "tinyengine_framebuffer.h"
#include "tinyengine_renderer_st7735r.h"
#include "tinyengine_audio.h"
#include "memory.h"
#include <malloc.h>

#include "jock.h"

#include "tinyengine_sprite.h"

#define BALL_CNT 30

tinyengine_handle_t engine = { 0 };
te_fb_handle_t frame_buf = { 0 };

tinyengine_status_t post_init() {
}

const char* txt = " hello world";

double counter = 0;
double x1 = 10, y1_ = 20, x2 = 0, y2 = 50;

struct ball {

    double x, y;
    double dirx, diry;
    double spd;
    uint8_t col;

} ball1, ball2, ball3;

static struct ball balls[BALL_CNT];

static uint16_t xs[BALL_CNT] = { 0 };
static uint16_t ys[BALL_CNT] = { 0 };

static te_sprite_t sprite_jockey;

tinyengine_status_t pre_init() {
    sprite_jockey.sprite_buffer = Docs;
    sprite_jockey.height = DOCS_HEIGHT;
    sprite_jockey.width = DOCS_WIDTH;

    for (uint8_t i = 0; i < BALL_CNT; i++) {
        balls[i].x = 200;
        xs[i] = balls[i].x;
        balls[i].y = 100;
        ys[i] = balls[i].y;
        balls[i].dirx = 2;
        balls[i].diry = 1.5;
        balls[i].spd = 100;
        balls[i].col = i;
    }
}


tinyengine_status_t render(double frameTime) {
    frame_buf.te_fb_wait_vsync_blocking_func_ptr();
    te_fb_clear(&frame_buf, 0x00);

    // for (uint8_t i = 0; i < BALL_CNT; i++) {
    //     // frame_buf.te_fb_wait_vsync_blocking_func_ptr();
    //     // te_fb_draw_filled_circle(&frame_buf, balls[i].x, balls[i].y, 10 + balls[i].y - balls[i].x, balls[i].col);
    // te_fb_draw_sprite(&frame_buf, &sprite_jockey, balls[0].x, balls[0].y);
    // }

    te_fb_draw_sprite_batch(&frame_buf, &sprite_jockey, xs, ys, BALL_CNT);

    // int x, y;
    // for (int i = 0; i < DOCS_HEIGHT * DOCS_WIDTH; i++) {
    //     x = i / DOCS_WIDTH;
    //     y = i % DOCS_WIDTH;
    //     te_fb_draw_pixel(&frame_buf, 50 - y, 50 + x, Docs[i]);

    // }

    // for (int i = 0; i < DOCS_HEIGHT * DOCS_WIDTH; i++) {
    //     x = i / DOCS_WIDTH;
    //     y = i % DOCS_WIDTH;
    //     te_fb_draw_pixel(&frame_buf, 100 - y, 100 + x, Docs[i]);

    // }

    // te_fb_draw_sprite(&frame_buf, &sprite_jockey, 100, 100);


    te_fb_swap_blocking(&frame_buf);

}

#define GRAVITY 5 // ~9.81

tinyengine_status_t update(double frameTime) {
    for (uint8_t i = 0; i < BALL_CNT; i++) {
        if (balls[i].x >= 320) {
            balls[i].dirx = -1 * ((double)get_rand_32() / (double)__RAND_MAX);
        }
        if (balls[i].y >= 240) {
            balls[i].diry = -1 * ((double)get_rand_32() / (double)__RAND_MAX);
        }
        if (balls[i].x <= 0) {
            balls[i].dirx = 1 * ((double)get_rand_32() / (double)__RAND_MAX);
        }
        if (balls[i].y <= 0) {
            balls[i].diry = 1 * ((double)get_rand_32() / (double)__RAND_MAX);
        }
        balls[i].x += balls[i].spd * frameTime * balls[i].dirx;
        balls[i].y += balls[i].spd * frameTime * balls[i].diry;
        xs[i] = balls[i].x;
        ys[i] = balls[i].y;
    }
    // if (balls[0].x >= 320) {
    //     balls[0].dirx = -1 * balls[0].spd;
    // }
    // if (balls[0].y >= 240) {
    //     balls[0].diry = -1 * balls[0].spd;
    // }

    // if (balls[0].x <= 0) {
    //     balls[0].dirx = 0;
    // }
    // if (balls[0].y <= 0) {
    //     balls[0].diry = 1 * GRAVITY;
    // }
    // balls[0].x += frameTime * balls[0].dirx;
    // balls[0].y += balls[0].spd * frameTime * balls[0].diry * -1 / GRAVITY;

}

tinyengine_status_t loop_clbk(double frameTime) {
}

uint32_t getTotalHeap(void) {
    extern char __StackLimit, __bss_end__;

    return &__StackLimit - &__bss_end__;
}

uint32_t getFreeHeap(void) {
    struct mallinfo m = mallinfo();

    return getTotalHeap() - m.uordblks;
}

extern char __StackLimit, __bss_end__;


void run() {

    te_audio_handle_t audio;
    audio.audio_out_1 = 20;

    te_audio_init(&audio);

    

    engine.lcd_spi_bklt = LCD_BKLT;
    engine.lcd_spi_clk = LCD_SPI_CLK;
    engine.lcd_spi_cs = LCD_CS;
    engine.lcd_spi_dc = LCD_DC;
    engine.lcd_spi_instance = LCD_SPI_PORT;
    engine.lcd_spi_miso = LCD_SPI_MISO;
    engine.lcd_spi_mosi = LCD_SPI_MOSI;
    engine.lcd_spi_speed = LCD_SPI_SPEED;
    engine.lcd_spi_rst = LCD_RST;
    engine.display_w = 640UL; // surprise UL is needed to define a unsigned long wich is a uint32_t; even if u implicit cast the compiler will not recognize the constant and cast it from a int to a uint32_t cauzing overflow.
    engine.display_h = 480UL;
    engine.pre_init_clbk = pre_init;
    engine.post_init_clbk = post_init;
    engine.update_clbk = update;
    engine.render_clbk = render;
    engine.loop_clbk = loop_clbk;

    tinyengine_init(&engine);

    // prepare the framebuffer dimenstions here.

    te_fb_init(&frame_buf, 320UL, 240UL, true);


    printf("Free Memory %d", getFreeHeap());

    te_fb_clear(&frame_buf, 0x00);
    // tinyengine_init_renderer(&engine, TINYENGINE_RENDERER_LCD_ST7735R);
    tinyengine_init_renderer(&engine, TINYENGINE_RENDERER_DVI, &frame_buf);
    // tinyengine_init_renderer(&engine, TINYENGINE_RENDERER_LCD_ILI9488);

    tinyengine_start(&engine);
    tinyengine_start_loop(&engine);
    te_fb_destroy(&frame_buf);
}