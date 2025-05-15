#include "main.h"

#include <stdio.h>

#include "hardware/spi.h"

#include "pico/stdlib.h"

#include "pico/rand.h"
#include "tinyengine.h"

#include "ST7735_TFT.h"
#include "hw.h"

#include "hardware/pll.h"
#include "hardware/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/clocks.h"


#define BALL_CNT 1

tinyengine_status_t post_init() {
}




const char* txt = " hello world";

double counter = 0;
double x1 = 10, y1 = 20, x2 = 0, y2 = 50;

struct ball {

    double x, y;
    double dirx, diry;
    double spd;

} ball1, ball2, ball3;

struct ball balls[BALL_CNT];

tinyengine_status_t pre_init() {
    // ball1.x = 2;
    // ball1.y = 10;
    // ball1.dirx = 0.1;
    // ball1.diry = 0.5;
    // ball1.spd = 500;



    // ball2.x = 2;
    // ball2.y = 10;
    // ball2.dirx = 0.1;
    // ball2.diry = 0.5;
    // ball2.spd = 500;

    for (uint8_t i = 0; i < BALL_CNT; i++) {
        balls[i].x = 2;
        balls[i].y = 10;
        balls[i].dirx = 0.1;
        balls[i].diry = 0.5;
        balls[i].spd = 500;
    }

}


char fps[32] = { 0 };

tinyengine_status_t render(double frameTime) {
    fillScreen(ST7735_BLACK);
    // fillCircle(x1, y1, 10, ST7735_BLUE);
    // fillCircle(x2, y1, 10, ST7735_RED);


    // fillCircle(ball1.x, ball1.y, 10, ST7735_GREEN);
    // fillCircle(ball2.x, ball2.y, 10, ST7735_RED);
    for (uint8_t i = 0; i < BALL_CNT; i++) {

        switch (i % 2)
        {
        case 1:
            fillCircle(balls[i].x, balls[i].y, 10, ST7735_GREEN);
            break;
        case 0:
            fillCircle(balls[i].x, balls[i].y, 10, ST7735_RED);
            break;
        default:
            break;
        }


    }
    // sprintf(fps, "fps: %d", frameTime);
    // drawText(0, 10, fps, ST7735_RED, ST7735_BLACK, 1);
}

tinyengine_status_t update(double frameTime) {
    // x1 = x1 + 50 * (frameTime * ((x1 >= 128) ? -1 : 1) * ((x1 <= 0) ? -1 : 1));
    // y1 = y1 + 50 * (frameTime * ((y1 >= 128) ? -1 : 1) * ((y1 <= 0) ? -1 : 1));
    // x2 = x2 + 50 * (frameTime * ((x2 >= 128) ? -1 : 1) * ((x2 <= 0) ? -1 : 1));
    // y2 = y2 + 50 * (frameTime * ((y2 >= 128) ? -1 : 1) * ((y2 <= 0) ? -1 : 1));
    // if (ball1.x >= 128) {
    //     ball1.dirx = -1 * ((double)get_rand_32() / (double)__RAND_MAX);
    // }
    // if (ball1.y >= 128) {
    //     ball1.diry = -1 * ((double)get_rand_32() / (double)__RAND_MAX);
    // }
    // if (ball1.x <= 0) {
    //     ball1.dirx = 1 * ((double)get_rand_32() / (double)__RAND_MAX);
    // }
    // if (ball1.y <= 0) {
    //     ball1.diry = 1 * ((double)get_rand_32() / (double)__RAND_MAX);
    // }

    // ball1.x += ball1.spd * frameTime * ball1.dirx;

    // ball1.y += ball1.spd * frameTime * ball1.diry;

    // if (ball2.x >= 128) {
    //     ball2.dirx = -1 * ((double)get_rand_32() / (double)__RAND_MAX);
    // }
    // if (ball2.y >= 128) {
    //     ball2.diry = -1 * ((double)get_rand_32() / (double)__RAND_MAX);
    // }
    // if (ball2.x <= 0) {
    //     ball2.dirx = 1 * ((double)get_rand_32() / (double)__RAND_MAX);
    // }
    // if (ball2.y <= 0) {
    //     ball2.diry = 1 * ((double)get_rand_32() / (double)__RAND_MAX);
    // }

    // ball2.x += ball2.spd * frameTime * ball2.dirx;

    // ball2.y += ball2.spd * frameTime * ball2.diry;


    for (uint8_t i = 0; i < BALL_CNT; i++) {
        if (balls[i].x >= 128) {
            balls[i].dirx = -1 * ((double)get_rand_32() / (double)__RAND_MAX);
        }
        if (balls[i].y >= 128) {
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

void measure_freqs(void) {
    uint f_pll_sys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY);
    uint f_pll_usb = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_USB_CLKSRC_PRIMARY);
    uint f_rosc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC);

    uint f_clk_sys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS);
    uint f_clk_peri = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_PERI);
    uint f_clk_usb = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_USB);
    uint f_clk_adc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_ADC);
#ifdef CLOCKS_FC0_SRC_VALUE_CLK_RTC
    uint f_clk_rtc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_RTC);
#endif

    printf("pll_sys  = %dkHz\n", f_pll_sys);
    printf("pll_usb  = %dkHz\n", f_pll_usb);
    printf("rosc     = %dkHz\n", f_rosc);
    printf("clk_sys  = %dkHz\n", f_clk_sys);
    printf("clk_peri = %dkHz\n", f_clk_peri);
    printf("clk_usb  = %dkHz\n", f_clk_usb);
    printf("clk_adc  = %dkHz\n", f_clk_adc);
#ifdef CLOCKS_FC0_SRC_VALUE_CLK_RTC
    printf("clk_rtc  = %dkHz\n", f_clk_rtc);
#endif

    // Can't measure clk_ref / xosc as it is the ref
}

int main() {
    set_sys_clock_khz(260000, true);
    stdio_init_all();

    measure_freqs();

    tinyengine_handle_t engine = {
        .lcd_spi_bklt = LCD_BKLT,
        .lcd_spi_clk = LCD_SPI_CLK,
        .lcd_spi_cs = LCD_CS,
        .lcd_spi_dc = LCD_DC,
        .lcd_spi_instance = LCD_SPI_PORT,
        .lcd_spi_miso = LCD_SPI_MISO,
        .lcd_spi_mosi = LCD_SPI_MOSI,
        .lcd_spi_speed = LCD_SPI_SPEED,
        .lcd_spi_rst = LCD_RST,
        .pre_init_clbk = pre_init,
        .post_init_clbk = post_init,
        .update_clbk = update,
        .render_clbk = render,
        .loop_clbk = loop_clbk
    };

    tinyengine_init(&engine);

    tinyengine_init_renderer(&engine, TINYENGINE_RENDERER_LCD_ST7735R);

    tinyengine_start(&engine);

    tinyengine_start_loop(&engine);

    // maybe shut down and reboot after?

    while (1) {
    }
    return 0;
}
