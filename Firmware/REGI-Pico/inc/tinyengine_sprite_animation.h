#ifndef __TINYENGINE_SPRITE_ANIMATION_H__
#define __TINYENGINE_SPRITE_ANIMATION_H__
#include "tinyengine_sprite.h"
#include "tinyengine.h"
#include "tinyengine_framebuffer.h"
// all frames must be the same dimentions.
typedef struct {

    uint8_t* sprite_animation_frames;
    uint16_t width, height;
    uint8_t animation_delay;
    uint8_t total_frames;
    uint8_t current_frame; // a presistant frame index.
    float current_frame_time; // a presistant frame time so we can count delay.

} te_sprite_animation_t;

// loops animation until it is not called and uses frame time to determine frame.
tinyengine_status_t te_sprite_render_animation(
    te_fb_handle_t* frame_buf, te_sprite_animation_t* sprite, uint16_t x, uint16_t y, float frameTime);

#endif