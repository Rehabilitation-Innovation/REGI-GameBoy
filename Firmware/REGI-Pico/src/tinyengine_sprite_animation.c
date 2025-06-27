#include "tinyengine.h"
#include "tinyengine_sprite_animation.h"

tinyengine_status_t te_sprite_render_animation(
    te_fb_handle_t* frame_buf, te_sprite_animation_t* sprite, uint16_t x, uint16_t y, float frameTime) {

    if (sprite->current_frame_time >= sprite->animation_delay) {
        sprite->current_frame += 1;
        sprite->current_frame_time = 0;
    }

    if (sprite->current_frame > sprite->total_frames - 1) {
        sprite->current_frame = 0;
    }

    sprite->current_frame_time += frameTime * 100;

    te_fb_draw_sprite_raw(
        frame_buf, &sprite->sprite_animation_frames[sprite->current_frame],
        sprite->width, sprite->height, x, y);

    return TINYENGINE_OK;
}