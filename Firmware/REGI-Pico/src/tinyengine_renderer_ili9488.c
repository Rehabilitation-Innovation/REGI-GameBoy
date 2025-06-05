#include "tinyengine_renderer_ili9488.h"
#include "DEV_Config.h"
#include "LCD_Driver.h"

tinyengine_status_t tinyengine_renderer_ili9488_init(tinyengine_handle_t* engine_handle) {

    DEV_GPIO_Init();
    spi_init(SPI_PORT, 62500000);
    gpio_set_function(LCD_CLK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(LCD_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(LCD_MISO_PIN, GPIO_FUNC_SPI);
    printf("spi1_baud %d \r\n", spi_get_baudrate(spi1));
    LCD_SCAN_DIR  lcd_scan_dir = SCAN_DIR_DFT;
    LCD_Init(lcd_scan_dir, 1000);

    LCD_Clear(TE_COLOR_BLUE);

    while (1) {
        LCD_Clear(TE_COLOR_BLUE);
        Driver_Delay_ms(100);
        LCD_Clear(TE_COLOR_CYAN);
        Driver_Delay_ms(100);
    }

    return TINYENGINE_OK;

}