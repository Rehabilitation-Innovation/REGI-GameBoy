#pragma once
#include "tinyengine_framebuffer.h"
/**
 * A 8bit color space sprite object. must be a 1-D array of 1 byte pixels.
 */
typedef struct {
    /**
     * ideally this should be a static array somewhere being pointed to.
     * Or be loaded from sd to external PSRAM in the future.
    */
    uint8_t* sprite_buffer; // un-used if animated.
    uint16_t width;
    uint16_t height;
    uint8_t scale_x;
    uint8_t scale_y;
    uint16_t rotation;
    // Unsuported currently will implement later.
    uint32_t* sprite_palette;

} te_sprite_t;

// all frames must be the same dimentions.
typedef struct {
    uint8_t** sprite_animation_frames;
    uint16_t width, height;
    uint8_t animation_delay;
    uint8_t total_frames;
    uint8_t current_frame; // a presistant frame index.
    float current_frame_time; // a presistant frame time so we can count delay.
    uint8_t start_frame, end_frame;
} te_sprite_animation_t;

class Sprite {
private:
    bool m_externaly_created = false;
    te_sprite_t* m_sprite_data = 0;
    bool m_animated = false;
    te_sprite_animation_t* m_animation_data = 0;
    double m_frametime = 0.0;
    uint16_t m_x = 0, m_y = 0;

public:
    [[nodiscard]] uint16_t get_x() const {
        return m_x;
    }

    void set_x(uint16_t x) {
        this->m_x = x;
    }

    [[nodiscard]] uint16_t get_y() const {
        return m_y;
    }

    void set_y(uint16_t y) {
        this->m_y = y;
    }

    Sprite(bool m_externaly_created, te_sprite_t *m_sprite_data, bool m_animated,
           te_sprite_animation_t *m_animation_data, double m_frametime, uint16_t x, uint16_t y)
        : m_externaly_created(m_externaly_created),
          m_sprite_data(m_sprite_data),
          m_animated(m_animated),
          m_animation_data(m_animation_data),
          m_frametime(m_frametime),
          m_x(x),
          m_y(y) {
    }

    Sprite() {};
    Sprite(te_sprite_t* _sprite_data);

    Sprite(te_sprite_t* _sprite_data, te_sprite_animation_t* _sprite_animation_data);

    ~Sprite();

    void render(TinyEngineFrameBuffer& _fb);

    // loops animation until it is not called and uses frame time to determine frame.
    // tinyengine_status_t te_sprite_render_animation(
    //     TinyEngineFrameBuffer* frame_buf, te_sprite_animation_t* sprite, uint16_t x, uint16_t y, float frameTime);


};