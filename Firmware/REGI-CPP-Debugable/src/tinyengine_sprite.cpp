#include "tinyengine.h"
#include "tinyengine_sprite.h"

Sprite::Sprite(te_sprite_t* _sprite_data) :
    m_sprite_data(_sprite_data)
{
};


Sprite::Sprite(te_sprite_t* _sprite_data, te_sprite_animation_t* _sprite_animation_data) :
    m_sprite_data(_sprite_data),
    m_animation_data(_sprite_animation_data)
{
    m_animated = true;
    m_externaly_created = true;
};

Sprite::~Sprite() {
    if (m_externaly_created) return;
    // only destroy if we own the data.
};

void Sprite::render(TinyEngineFrameBuffer& _fb) {
    if (!m_animated) {
        _fb.draw_sprite_raw(
            m_sprite_data->sprite_buffer,
            m_sprite_data->width,
            m_sprite_data->height,
            m_x,
            m_y);
        return;
    }

    if (m_animation_data->current_frame_time >= m_animation_data->animation_delay) {
        m_animation_data->current_frame += 1;
        m_animation_data->current_frame_time = 0;
    }

    if (m_animation_data->current_frame > m_animation_data->total_frames - 1
        || m_animation_data->current_frame >= m_animation_data->end_frame) {
        m_animation_data->current_frame = m_animation_data->start_frame;
    }

    m_animation_data->current_frame_time += m_frametime * 100;

    _fb.draw_sprite_raw(
        m_animation_data->sprite_animation_frames[m_animation_data->current_frame],
        m_animation_data->width, m_animation_data->height, m_x, m_y);

};




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