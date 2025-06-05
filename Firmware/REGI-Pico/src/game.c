#include "game.h"

#include <stdio.h>
#include "tinyengine.h"
#include "tinyengine_framebuffer.h"
#include "tinyengine_renderer_st7735r.h"
#include "hardware/spi.h"
#include <math.h>
#include "pico/stdlib.h"

#include "pico/rand.h"
#include "tinyengine.h"

#define BALL_CNT 10

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

} ball1, ball2, ball3;

struct ball balls[BALL_CNT];

tinyengine_status_t pre_init() {
    for (uint8_t i = 0; i < BALL_CNT; i++) {
        balls[i].x = 2;
        balls[i].y = 10;
        balls[i].dirx = 1;
        balls[i].diry = 0.5;
        balls[i].spd = 100;
    }
}

tinyengine_status_t render(double frameTime) {


    for (uint8_t i = 0; i < BALL_CNT; i++) {
        te_fb_draw_filled_circle(&frame_buf, balls[i].x, balls[i].y, 10, 0xF0);
    }

    sleep_ms(10);
    for (uint8_t i = 0; i < BALL_CNT; i++) {
        te_fb_draw_filled_circle(&frame_buf, balls[i].x, balls[i].y, 10, 0x00);
    }

}

tinyengine_status_t update(double frameTime) {
    for (uint8_t i = 0; i < BALL_CNT; i++) {
        if (balls[i].x >= 640) {
            balls[i].dirx = -1 * ((double)get_rand_32() / (double)__RAND_MAX);
        } 
        if (balls[i].y >= 480) {
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
    }
}

tinyengine_status_t loop_clbk(double frameTime) {
}

void run() {

    engine.lcd_spi_bklt = LCD_BKLT;
    engine.lcd_spi_clk = LCD_SPI_CLK;
    engine.lcd_spi_cs = LCD_CS;
    engine.lcd_spi_dc = LCD_DC;
    engine.lcd_spi_instance = LCD_SPI_PORT;
    engine.lcd_spi_miso = LCD_SPI_MISO;
    engine.lcd_spi_mosi = LCD_SPI_MOSI;
    engine.lcd_spi_speed = LCD_SPI_SPEED;
    engine.lcd_spi_rst = LCD_RST;
    engine.display_w = 640UL; // surprise
    engine.display_h = 480UL;
    engine.pre_init_clbk = pre_init;
    engine.post_init_clbk = post_init;
    engine.update_clbk = update;
    engine.render_clbk = render;
    engine.loop_clbk = loop_clbk;

    tinyengine_init(&engine);
    te_fb_init(&frame_buf, &engine);
    te_fb_clear(&frame_buf, 0x00);
    // tinyengine_init_renderer(&engine, TINYENGINE_RENDERER_LCD_ST7735R);
    tinyengine_init_renderer(&engine, TINYENGINE_RENDERER_DVI, frame_buf.pixel_buffer, frame_buf.pixel_buffer_size);
    // tinyengine_init_renderer(&engine, TINYENGINE_RENDERER_LCD_ILI9488);

    tinyengine_start(&engine);
    tinyengine_start_loop(&engine);
    te_fb_destroy(&frame_buf);
}