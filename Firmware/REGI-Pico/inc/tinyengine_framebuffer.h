#ifndef __TINYENGINE_FRAMEBUFFER_H__
#define __TINYENGINE_FRAMEBUFFER_H__

#include "tinyengine.h"
#include "tinyengine_sprite.h"
#ifndef TE_FB_PIXEL_COLOR_DEPTH_BITS
#define TE_FB_PIXEL_COLOR_DEPTH_BITS 16
#endif

// @TODO: add presets for resolutions, for now just use 320x240x2bpp ; RBG555
typedef void(*te_fb_swap_blocking_func_t)();
/**
 * No need to edit this struct te_fb_init will return a filled struct.
 */
typedef struct {

    uint8_t* pixel_buffer_display;
    uint8_t* pixel_buffer_back;
    uint32_t pixel_buffer_size; // a quicker look up. rather than calculating every time; includes color depth
    uint32_t display_w;
    uint32_t display_h;

    te_fb_swap_blocking_func_t te_fb_wait_vsync_blocking_func_ptr;

    int is_dual;

} te_fb_handle_t;

tinyengine_status_t te_fb_init(te_fb_handle_t* fb_handle, uint32_t frame_width, uint32_t frame_height, int is_dual);

tinyengine_status_t te_fb_destroy(te_fb_handle_t* fb_handle);

tinyengine_status_t te_fb_write(te_fb_handle_t* fb_handle);

tinyengine_status_t te_fb_clear(te_fb_handle_t* fb_handle, uint8_t color);

tinyengine_status_t te_fb_draw_pixel(te_fb_handle_t* fb_handle, uint32_t x, uint32_t y, uint8_t color);

tinyengine_status_t te_fb_draw_outline_rectangle(te_fb_handle_t* fb_handle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t color);

tinyengine_status_t te_fb_draw_filled_rectangle(te_fb_handle_t* fb_handle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t color);

tinyengine_status_t te_fb_draw_outline_circle(te_fb_handle_t* fb_handle, uint32_t x, uint32_t y, uint32_t r, uint8_t color);

tinyengine_status_t te_fb_draw_filled_circle(te_fb_handle_t* fb_handle, uint32_t x, uint32_t y, uint32_t r, uint8_t color);

tinyengine_status_t te_fb_swap_blocking(te_fb_handle_t* fb_handle);

tinyengine_status_t te_fb_draw_sprite(te_fb_handle_t* fb_handle, te_sprite_t* sprite, uint32_t x, uint32_t y);

tinyengine_status_t te_fb_draw_sprite_raw(te_fb_handle_t* fb_handle, uint8_t* sprite, uint32_t w, uint32_t h, uint32_t x, uint32_t y);

tinyengine_status_t te_fb_draw_sprite_batch(te_fb_handle_t* fb_handle, te_sprite_t* sprite, uint16_t x[], uint16_t y[], uint16_t count);


#endif