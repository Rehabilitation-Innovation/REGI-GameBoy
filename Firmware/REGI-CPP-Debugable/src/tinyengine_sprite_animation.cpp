#include "tinyengine.h"
#include "tinyengine_sprite_animation.h"
extern "C" {
    tinyengine_status_t te_sprite_render_animation(
        TinyEngineFrameBuffer& frame_buf, te_sprite_animation_t* sprite, uint16_t x, uint16_t y, float frameTime) {

        if (sprite->current_frame_time >= sprite->animation_delay) {
            sprite->current_frame += 1;
            sprite->current_frame_time = 0;
        }

        if (sprite->current_frame > sprite->total_frames - 1) {
            sprite->current_frame = 0;
        }

        sprite->current_frame_time += frameTime * 100;

        frame_buf.draw_sprite_raw(
            sprite->sprite_animation_frames[sprite->current_frame],
            sprite->width, sprite->height, x, y);

        return TINYENGINE_OK;
    }
}