#pragma once
#include "tinyengine.h"
#ifndef PIXEL_COLOR_DEPTH_BITS
#define PIXEL_COLOR_DEPTH_BITS 16
#endif

// @TODO: add presets for resolutions, for now just use 320x240x2bpp ; RBG555
// typedef void(*swap_blocking_func_t)();
/**
 * No need to edit this struct init will return a filled struct.
 */
 // typedef struct {
class TinyEngineFrameBuffer {
private:
    uint8_t* pixel_buffer_back;
    uint32_t m_display_w;
    uint32_t m_display_h;
    int m_is_dual;
    tinyengine_status_t init();

public:

    uint8_t* pixel_buffer_display;
    uint32_t m_pixel_buffer_size; // a quicker look up. rather than calculating every time; includes color depth

    bool is_dual() { return m_is_dual; };

    void drawCircle1(int xc, int yc, int x, int y, uint8_t color);

    TinyEngineFrameBuffer(uint32_t frame_width, uint32_t frame_height, int is_dual) : m_display_h(frame_height), m_display_w(frame_width), m_is_dual(is_dual) { init(); };

    ~TinyEngineFrameBuffer() {};

    tinyengine_status_t destroy();

    tinyengine_status_t write();

    tinyengine_status_t clear(uint8_t color);

    tinyengine_status_t draw_pixel(uint32_t x, uint32_t y, uint8_t color);

    tinyengine_status_t draw_outline_rectangle(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t color);

    tinyengine_status_t draw_filled_rectangle(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t color);

    tinyengine_status_t draw_outline_circle(uint32_t x, uint32_t y, uint32_t r, uint8_t color);

    tinyengine_status_t draw_filled_circle(uint32_t x, uint32_t y, uint32_t r, uint8_t color);

    tinyengine_status_t swap_blocking();

    tinyengine_status_t draw_sprite_raw(uint8_t* sprite, uint32_t w, uint32_t h, uint32_t x, uint32_t y);

    tinyengine_status_t draw_sprite_raw_batch(uint8_t* sprite, uint32_t w, uint32_t h, uint16_t x[], uint16_t y[], uint16_t count);

};