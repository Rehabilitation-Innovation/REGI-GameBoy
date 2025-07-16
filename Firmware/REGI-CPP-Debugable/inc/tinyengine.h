#pragma once
#include "pico/stdlib.h"
#include "stdio.h"
#include "hardware/spi.h"
#include <stdint.h>
#include "tinyengine_scene.h"
#include <unordered_map>

#include <functional>

class TinyEngineFrameBuffer;
class TinyEngineRendererI;
class TinyEngineScene;
class Sprite;

#define telog(M, ...) \
  printf("[Tiny Engine Log] (%s:%d)" M "\r\n", __BASE_FILE__, __LINE__, ##__VA_ARGS__)

#define teerr(M, ...) \
  printf("[Tiny Engine ERROR] (%s:%d)" M "\r\n", __BASE_FILE__, __LINE__, ##__VA_ARGS__)

typedef enum {
    TINYENGINE_OK,
    TINYENGINE_ERROR,
    TINYENGINE_RENDER_ERROR,
    TINYENGINE_OUTOFBOUNDS_ERROR
} tinyengine_status_t;

#include "tinyengine_framebuffer.h"

typedef enum {
    TINYENGINE_RENDERER_DVI,
    // TINYENGINE_RENDERER_LCD_ST7735R,  // 1.8 in adafruit green tab with sdcard
    // TINYENGINE_RENDERER_LCD_ILI9488,  // 3.5 in waveshare dev kit with sdcard
} tinyengine_renderer_t;

typedef tinyengine_status_t(*tinyengine_init_clbk_t)();


typedef void(*te_draw_pixel_func_t)(uint8_t* pixel_buffer, uint32_t len);

typedef struct {
    tinyengine_renderer_t current_output_renderer;
    te_draw_pixel_func_t set_buffer;
} tinyengine_renderer_handle_t;

// The reason this is a uint8_t* in the paramenter is that its defined before the engine handle
// but it needs access to it. So to give it access i give it a uint8_t* instead of struct*
typedef tinyengine_status_t(*tinyengine_clbk_t)(double delta);

class TinyEngineRendererI {
public:
    virtual tinyengine_status_t swap_blocking() = 0;
    virtual tinyengine_status_t swap_non_blocking() = 0;
    virtual tinyengine_status_t wait_for_vsync() = 0;
    virtual tinyengine_status_t tinyengine_renderer_init() = 0;
};

class TinyEngine {
private:
    spi_inst_t* lcd_spi_instance;
    uint8_t lcd_spi_clk;
    uint8_t lcd_spi_miso;
    uint8_t lcd_spi_mosi;
    uint8_t lcd_spi_dc;
    uint8_t lcd_spi_rst;
    uint8_t lcd_spi_cs;
    uint8_t lcd_spi_bklt;
    uint32_t lcd_spi_speed;

    uint32_t display_w;
    uint32_t display_h;
    uint16_t fps;
    // std::vector<SceneI&> scenes; // add suport for multiple scenes and switching between them
    std::function<tinyengine_status_t(void)> pre_init_clbk;

    std::function<tinyengine_status_t(void)> post_init_clbk;
    std::function<tinyengine_status_t(void)> loop_clbk;
    std::function<tinyengine_status_t(void)> render_clbk;
    std::function<tinyengine_status_t(double)> update_clbk;
    // std::function<tinyengine_status_t(void)> input_clbk;
    void update_inputs();

    std::unordered_map<uint8_t, std::function<void()>> m_gpio_input_events;
    std::unordered_map<uint8_t, std::function<void()>> m_serial_input_events;
    std::vector<uint8_t> m_gpio_input_buffer;
    std::vector<uint8_t> m_serial_input_buffer;

    uint32_t profile_main_loop_time = 0;
    uint32_t profile_render_loop_time = 0;
    uint32_t profile_update_loop_time = 0;

public:
    TinyEngineRendererI& renderer;

    TinyEngine(TinyEngineRendererI& rn) : renderer(rn) {
    };

    uint16_t get_fps() const
    {
        return fps;
    }

    void set_pre_inti_clbk(std::function<tinyengine_status_t(void)> _pre_init_clbk) { pre_init_clbk = _pre_init_clbk; };
    void set_post_init_clbk(std::function<tinyengine_status_t(void)> _post_init_clbk) { post_init_clbk = _post_init_clbk; };
    void set_loop_clbk(std::function<tinyengine_status_t(void)> _loop_clbk) { loop_clbk = _loop_clbk; };
    void set_render_clbk(std::function<tinyengine_status_t(void)> _render_clbk) { render_clbk = _render_clbk; };
    void set_update_clbk(std::function<tinyengine_status_t(double)> _update_clbk) { update_clbk = _update_clbk; };

    tinyengine_status_t init();
    tinyengine_status_t start();
    tinyengine_status_t start_loop();

    void render(double frametime);
    void update(double frametime);
    void set_scene(SceneI& scene) {};

    void bind_gpio_input_event(uint8_t _gpio, std::function<void()> _event_callback);
    void bind_serial_input_event(uint8_t _char, std::function<void()> _event_callback);

    uint32_t get_profile_main_loop_time() const {
        return profile_main_loop_time;
    }

    uint32_t get_profile_render_loop_time() const {
        return profile_render_loop_time;
    }

    uint32_t get_profile_update_loop_time() const {
        return profile_update_loop_time;
    }
};

#define TE_COLOR_BLUE  0x001F  /**< Blue */
#define TE_COLOR_RED  0xF800  /**< Red */
#define TE_COLOR_GREEN  0x07E0  /**< Green */
#define TE_COLOR_CYAN  0x07FF  /**< Cyan */
#define TE_COLOR_MAGENTA  0xF81F  /**< Magenta */
#define TE_COLOR_YELLOW  0xFFE0  /**< Yellow */
#define TE_COLOR_WHITE  0xFFFF  /**< White */
#define TE_COLOR_TAN  0xED01  /**< Tan */
#define TE_COLOR_GREY  0x9CD1  /**< Grey */
#define TE_COLOR_BROWN  0x6201  /**< Brown */
#define TE_COLOR_DGREEN  0x01C0  /**< Dark Green */
#define TE_COLOR_ORANGE  0xFC00  /**< Orange */
#define TE_COLOR_NAVY  0x000F  /**< Navy */
#define TE_COLOR_DCYAN  0x03EF  /**< Dark Cyan */
#define TE_COLOR_MAROON  0x7800  /**< Maroon */
#define TE_COLOR_PURPLE  0x780F  /**< Purple */
#define TE_COLOR_OLIVE  0x7BE0  /**< Olive */
#define TE_COLOR_LGREY  0xC618  /**< Light Grey */
#define TE_COLOR_DGREY  0x7BEF  /**< Dark Grey */
#define TE_COLOR_GYELLOW  0xAFE5  /**< Greenish Yellow */
#define TE_COLOR_PINK  0xFC18  /**< Pink */
#define TE_COLOR_LBLUE  0x7E5F  /**< Light Blue */
#define TE_COLOR_BLACK  0x0000  /**< Black */
#define TE_COLOR_BEIGE  0xB5D2  /**< Beige */
