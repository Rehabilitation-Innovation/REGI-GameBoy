#include "tinyengine.h"
#include "tinyengine_sprite.h"

Sprite::Sprite(te_sprite_t* _sprite_data) :
    m_sprite_data(_sprite_data)
{
};

Sprite::Sprite(te_sprite_t _sprite_data)
{
    m_sprite_data = new te_sprite_t{
        .sprite_buffer = _sprite_data.sprite_buffer,
        .width = _sprite_data.width,
        .height = _sprite_data.height,
        .sprite_palette = _sprite_data.sprite_palette,
    };

    m_externaly_created = false;
}

Sprite::Sprite(uint8_t* sprite_buffer, uint16_t width, uint16_t height)
{
    m_sprite_data = new te_sprite_t{
        .sprite_buffer = sprite_buffer,
        .width = width,
        .height = height,
        .sprite_palette = 0
    };
    m_externaly_created = true;
}

Sprite::Sprite(te_sprite_t* _sprite_data, te_sprite_animation_t* _sprite_animation_data) :
    m_sprite_data(_sprite_data),
    m_animation_data(_sprite_animation_data)
{
    m_animated = true;
    m_externaly_created = true;
};

Sprite::Sprite(const te_sprite_t& _sprite_data, const te_sprite_animation_t& _sprite_animation_data)
{
    m_sprite_data = new te_sprite_t{
        .sprite_buffer = _sprite_data.sprite_buffer,
        .width = _sprite_data.width,
        .height = _sprite_data.height,
        .sprite_palette = _sprite_data.sprite_palette,
    };
    m_animation_data = new te_sprite_animation_t{
        .sprite_animation_frames = _sprite_animation_data.sprite_animation_frames,
        .animation_delay = _sprite_animation_data.animation_delay,
        .total_frames = _sprite_animation_data.total_frames,
        .current_frame = _sprite_animation_data.current_frame,
        .start_frame = _sprite_animation_data.start_frame,
        .end_frame = _sprite_animation_data.end_frame,
    };
    m_externaly_created = false;
}

Sprite::~Sprite()
{
    if (m_externaly_created) return;
    // only destroy if we own the data.
};

void Sprite::render(TinyEngineFrameBuffer& _fb)
{

};