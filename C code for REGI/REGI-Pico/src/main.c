#include "main.h"

#include <stdio.h>

#include "hardware/spi.h"

#include "pico/stdlib.h"

#include "tinyengine.h"

#include "ST7735_TFT.h"
#include "hw.h"

#include "hardware/pll.h"
#include "hardware/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/clocks.h"


tinyengine_status_t post_init() {
}

tinyengine_status_t pre_init() {
}

tinyengine_status_t update(double frameTime) {
}

const char* txt = " hello world";

tinyengine_status_t render(double frameTime) {



    // static int a = 1;
    // a = !a;
    // if (a)
    //     fillScreen(ST7735_BLACK);
    // else
    //     // static double time = 0;
    //     // static double time2 = 0;
    //     // if ((time2 - time) >= 10)
    //     // time = frameTime;
    //     fillScreen(ST7735_BLUE);


    drawText(0, 5, txt, ST7735_WHITE, ST7735_BLACK, 1);
    drawText(0, 15, txt, ST7735_BLUE, ST7735_BLACK, 1);
    drawText(0, 25, txt, ST7735_RED, ST7735_BLACK, 1);
    drawText(0, 35, txt, ST7735_GREEN, ST7735_BLACK, 1);
    drawText(0, 45, txt, ST7735_CYAN, ST7735_BLACK, 1);
    drawText(0, 55, txt, ST7735_MAGENTA, ST7735_BLACK, 1);
    drawText(0, 65, txt, ST7735_YELLOW, ST7735_BLACK, 1);
    drawText(0, 75, txt, ST7735_WHITE, ST7735_BLACK, 1);

    fillScreen(ST7735_BLACK);

    // drawText(0, 5, " !#$%&'()*+,-.", ST7735_WHITE, ST7735_BLACK, 1);
    // drawText(0, 15, "0123456789", ST7735_BLUE, ST7735_BLACK, 1);
    // drawText(0, 25, "abcdefghijklmn", ST7735_RED, ST7735_BLACK, 1);
    // drawText(0, 35, "ABCDEGHIJKLMN", ST7735_GREEN, ST7735_BLACK, 1);
    // drawText(0, 45, "opqrstuvwxyz", ST7735_CYAN, ST7735_BLACK, 1);
    // drawText(0, 55, "OPQRSTUVWYXZ", ST7735_MAGENTA, ST7735_BLACK, 1);
    // drawText(0, 65, ";:=,.?@", ST7735_YELLOW, ST7735_BLACK, 1);
    // drawText(0, 75, "[]/", ST7735_BLACK, ST7735_WHITE, 1);

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
