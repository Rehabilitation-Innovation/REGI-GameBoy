#pragma once
#include "tinyengine_framebuffer.h"
/**
 * A 8bit color space sprite object. must be a 1-D array of 1 byte pixels.
 */
typedef struct
{
    /**
     * ideally this should be a static array somewhere being pointed to.
     * Or be loaded from sd to external PSRAM in the future.
    */
    uint8_t* sprite_buffer; // un-used if animated.
    uint16_t width;
    uint16_t height;
    // Unsuported currently will implement later.
    uint32_t* sprite_palette;
} te_sprite_t;

// all frames must be the same dimentions.
typedef struct
{
    uint8_t** sprite_animation_frames;
    uint8_t animation_delay;
    uint8_t total_frames;
    uint8_t current_frame; // a presistant frame index.
    uint8_t start_frame, end_frame;
} te_sprite_animation_t;

class Sprite
{
private:
    bool m_externaly_created = false;
    te_sprite_t* m_sprite_data = 0;
    bool m_animated = false;
    te_sprite_animation_t* m_animation_data = 0;
    double m_frametime = 0.0;
    uint8_t m_animation_speed = 100;
    // its debateble that this should be a data structure since all the frame times are updated at the same time. but we can also say that some sprites may be frozen.
    uint16_t m_x = 0, m_y = 0;
    uint8_t m_scale_x = 0, m_scale_y = 0;

public:
    Sprite(bool m_externaly_created, te_sprite_t* m_sprite_data, bool m_animated,
           te_sprite_animation_t* m_animation_data, double m_frametime, uint16_t x, uint16_t y)
        : m_externaly_created(m_externaly_created),
          m_sprite_data(m_sprite_data),
          m_animated(m_animated),
          m_animation_data(m_animation_data),
          m_frametime(m_frametime),
          m_x(x),
          m_y(y)
    {
    }

    Sprite()
    {
    };

    Sprite(uint8_t* sprite_buffer, uint16_t width, uint16_t height);

    Sprite(te_sprite_t _sprite_data);
    Sprite(te_sprite_t* _sprite_data);

    Sprite(te_sprite_t* _sprite_data, te_sprite_animation_t* _sprite_animation_data);
    Sprite(const te_sprite_t& _sprite_data, const te_sprite_animation_t& _sprite_animation_data);

    ~Sprite();

    void render(TinyEngineFrameBuffer& _fb);

    bool is_m_externaly_created() const
    {
        return m_externaly_created;
    }

    void set_m_externaly_created(const bool m_externaly_created)
    {
        this->m_externaly_created = m_externaly_created;
    }

    te_sprite_t* get_m_sprite_data()
    {
        return m_sprite_data;
    }

    void set_m_sprite_data(te_sprite_t* const m_sprite_data)
    {
        this->m_sprite_data = m_sprite_data;
    }

    bool is_m_animated() const
    {
        return m_animated;
    }

    void set_m_animated(const bool m_animated)
    {
        this->m_animated = m_animated;
    }

    te_sprite_animation_t* get_m_animation_data()
    {
        return m_animation_data;
    }

    void set_m_animation_data(te_sprite_animation_t* const m_animation_data)
    {
        this->m_animation_data = m_animation_data;
    }

    double get_m_frametime() const
    {
        return m_frametime;
    }

    void set_m_frametime(const double m_frametime)
    {
        this->m_frametime += m_frametime;
    }

    uint16_t get_m_x() const
    {
        return m_x;
    }

    void set_m_x(const uint16_t m_x)
    {
        this->m_x = m_x;
    }

    uint16_t get_m_y() const
    {
        return m_y;
    }

    void set_m_y(const uint16_t m_y)
    {
        this->m_y = m_y;
    }

    uint8_t get_m_scale_x() const
    {
        return m_scale_x;
    }

    void set_m_scale_x(const uint8_t m_scale_x)
    {
        this->m_scale_x = m_scale_x;
    }

    uint8_t get_m_scale_y() const
    {
        return m_scale_y;
    }

    void set_m_scale_y(const uint8_t m_scale_y)
    {
        this->m_scale_y = m_scale_y;
    }

    uint8_t get_m_animation_speed() const
    {
        return m_animation_speed;
    }

    void set_m_animation_speed(uint8_t m_animation_speed)
    {
        this->m_animation_speed = m_animation_speed;
    }
};
