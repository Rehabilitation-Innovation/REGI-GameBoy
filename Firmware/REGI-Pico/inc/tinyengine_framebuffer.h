#ifndef __TINYENGINE_FRAMEBUFFER_H__
#define __TINYENGINE_FRAMEBUFFER_H__

#include "tinyengine.h"

#ifndef TE_FB_PIXEL_COLOR_DEPTH_BITS
#define TE_FB_PIXEL_COLOR_DEPTH_BITS 16
#endif

// @TODO: add presets for resolutions, for now just use 320x240x2bpp ; RBG555

/**
 * No need to edit this struct te_fb_init will return a filled struct.
 */
typedef struct {

    uint8_t* pixel_buffer;
    uint8_t* pixel_buffer_back;
    uint32_t pixel_buffer_size; // a quicker look up. rather than calculating every time; includes color depth
    uint32_t display_w;
    uint32_t display_h;

    tinyengine_renderer_handle_t renderer;

} te_fb_handle_t;

tinyengine_status_t te_fb_init(te_fb_handle_t* fb_handle, tinyengine_handle_t* engine);

tinyengine_status_t te_fb_destroy(te_fb_handle_t* fb_handle);

tinyengine_status_t te_fb_write(te_fb_handle_t* fb_handle);

tinyengine_status_t te_fb_clear(te_fb_handle_t* fb_handle, uint16_t color);

tinyengine_status_t te_fb_draw_pixel(te_fb_handle_t* fb_handle, uint32_t x, uint32_t y, uint16_t color);

tinyengine_status_t te_fb_draw_outline_rectangle(te_fb_handle_t* fb_handle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint16_t color);

tinyengine_status_t te_fb_draw_filled_rectangle(te_fb_handle_t* fb_handle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint16_t color);

tinyengine_status_t te_fb_draw_outline_circle(te_fb_handle_t* fb_handle, uint32_t x, uint32_t y, uint32_t r, uint16_t color);

tinyengine_status_t te_fb_draw_filled_circle(te_fb_handle_t* fb_handle, uint32_t x, uint32_t y, uint32_t r, uint16_t color);

#endif