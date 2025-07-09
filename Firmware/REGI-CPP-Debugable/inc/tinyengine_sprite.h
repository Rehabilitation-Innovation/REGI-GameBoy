#pragma once
#include "tinyengine.h"
#include "tinyengine_framebuffer.h"
/**
 * A 8bit color space sprite object. must be a 1-D array of 1 byte pixels.
 */
typedef struct {
    /**
     * ideally this should be a static array somewhere being pointed to.
     * Or be loaded from sd to external PSRAM in the future.
    */
    uint8_t* sprite_buffer; // un used if animated.
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

} te_sprite_animation_t;



class Sprite {
private:
    bool m_externaly_created = false;
    te_sprite_t* m_sprite_data = 0;
    bool m_animated = false;
    te_sprite_animation_t* m_animation_data = 0;
    TinyEngineFrameBuffer& m_framebuffer;
    double m_frametime;
    uint16_t x, y = { 0 };
public:
    Sprite(te_sprite_t* _sprite_data, TinyEngineFrameBuffer& _framebuffer);

    Sprite(te_sprite_t* _sprite_data, te_sprite_animation_t* _sprite_animation_data, TinyEngineFrameBuffer& _framebuffer);

    ~Sprite();

    void render();

    // loops animation until it is not called and uses frame time to determine frame.
    // tinyengine_status_t te_sprite_render_animation(
    //     TinyEngineFrameBuffer* frame_buf, te_sprite_animation_t* sprite, uint16_t x, uint16_t y, float frameTime);


};