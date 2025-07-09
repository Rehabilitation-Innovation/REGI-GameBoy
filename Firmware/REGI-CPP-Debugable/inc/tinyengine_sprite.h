#ifndef __TINYENGINE_SPRITE_H__
#define __TINYENGINE_SPRITE_H__

/**
 * A 8bit color space sprite object. must be a 1-D array of 1 byte pixels.
 */
typedef struct {
    /**
     * ideally this should be a static array somewhere being pointed to.
     * Or be loaded from sd to external PSRAM in the future.
    */
    uint8_t* sprite_buffer;

    uint16_t width;
    uint16_t height;

    uint16_t scale_x;

    uint16_t scale_y;

    uint16_t rotation;

    // Unsuported currently will implement later.
    uint32_t* sprite_palette;

} te_sprite_t;



#endif