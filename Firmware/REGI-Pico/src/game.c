#include <stdio.h>
#include <math.h>

#include "game.h"
#include "pico/stdlib.h"
#include "pico/rand.h"
#include "hardware/pwm.h"

#include "pwm-tone.h"   // Include the library
#include "melodies.h"   // Optional, but ideal location to store custom melodies

#include "tinyengine.h"
#include "tinyengine_renderer.h"
#include "tinyengine_framebuffer.h"
#include "tinyengine_renderer_st7735r.h"
#include "tinyengine_audio.h"
#include "memory.h"
#include <malloc.h>

#include "jock.h"
#include "dino_running.h"
#include "cloud.h"
#include <math.h>
#include "tinyengine_sprite.h"
#include "tinyengine_sprite_animation.h"

#define BALL_CNT 30
#define PIEZO_PIN 20

#define GRAVITY 0.98f

tinyengine_handle_t engine = { 0 };
te_fb_handle_t frame_buf = { 0 };

tinyengine_status_t post_init() {
}

const char* txt = " hello world";

struct ball {

    float x, y;
    float dirx, diry;
    float spd;
    uint8_t col;

} ball1, ball2, ball3;

static struct ball balls[BALL_CNT];

static uint16_t xs[BALL_CNT] = { 0 };
static uint16_t ys[BALL_CNT] = { 0 };

static uint32_t charecterCurrentx = 0;
static uint32_t charecterCurrenty = 0;

static te_sprite_t sprite_jockey;

static te_sprite_t sprite_cloud = {
    .height = CLOUD_HEIGHT,
    .width = CLOUD_WIDTH,
    .sprite_buffer = cloud
};

static uint8_t* epd_bitmap_allArray[11] = {
    tile000,
    tile001,
    tile002,
    tile003,
    tile004,
    tile005,
    tile006,
    tile007,
    tile008,
    tile009,
    tile010
};

uint8_t* dinos[2] = { 0 };

te_sprite_animation_t dino_ani = {
    .animation_delay = 2,
    .current_frame = 0,
    .current_frame_time = 0,
    .height = 24,
    .width = 24,
    .sprite_animation_frames = epd_bitmap_allArray,
    .total_frames = 11
};

tinyengine_status_t pre_init() {
    sprite_jockey.sprite_buffer = tile000;
    sprite_jockey.height = 48;
    sprite_jockey.width = 48;

    dinos[0] = tile000;
    dinos[1] = tile006;

    // for (uint8_t i = 0; i < 2; i++)
    // {
    //     dinos[i].sprite_buffer = epd_bitmap_allArray[i];
    //     dinos[i].height = 48;
    //     dinos[i].width = 48;
    // }
}

float clx = 0, cly = 0;
// te_sprite_t* dino = &dinos[0];
tinyengine_status_t render(double frameTime) {
    frame_buf.te_fb_wait_vsync_blocking_func_ptr();
    te_fb_clear(&frame_buf, 0x00);

    // te_fb_draw_sprite(&frame_buf, &sprite_cloud, clx + CLOUD_WIDTH / 2, cly);
    // te_fb_draw_sprite(&frame_buf, dino, 100, 100);
    te_sprite_render_animation(&frame_buf, &dino_ani, 100, 100, frameTime);
    // te_fb_draw_sprite(&frame_buf, &sprite_jockey, 100, 100);
    te_fb_swap_blocking(&frame_buf);
}


tinyengine_status_t update(double frameTime) {
    // 320 x 240    int charecter_Sprite_center_x = DOCS_WIDTH / 2 + charecterCurrentx;
    // int charecter_Sprite_center_y = DOCS_HEIGHT / 2 + char;
    // static uint32_t
    //     charharecterCucharecterCurrenty;DOCS_HEIGHT / 2 + charecterCurrenty;


    clx -= 100 * frameTime;

    if ((clx + CLOUD_WIDTH) <= 0) clx = frame_buf.display_w;

    // static int i = 0;
    // static float count = 0;

    // count += frameTime * 10;

    // if (count >= 5) {
    //     i += 1;
    //     count = 0;
    // }

    // if (i > 1) {
    //     i = 0;
    // }

    // dino = &dinos[i];

}

tinyengine_status_t loop_clbk(double frameTime) {
}

uint32_t getTotalHeap(void) {
    extern char __StackLimit, __bss_end__;

    return &__StackLimit - &__bss_end__;
}

uint32_t getFreeHeap(void) {
    struct mallinfo m = mallinfo();

    return getTotalHeap() - m.uordblks;
}

extern char __StackLimit, __bss_end__;

tonegenerator_t generator;
void run() {

    te_audio_handle_t audio;
    audio.audio_out_1 = 20;
    audio.period = 10000;

    // te_audio_init(&audio);

    // tone_init(&generator, PIEZO_PIN);
    // tone(&generator, NOTE_A4, 200);
    // while (generator.playing) { sleep_ms(2); }
    // tone(&generator, NOTE_A5, 200);
    // sleep_ms(500);
    // melody(&generator, RINGTONE_1, 3);
    // while (generator.playing) { sleep_ms(2); }
    // sleep_ms(500);

    // melody(&generator, HAPPY_BIRTHDAY, 0);
    // while (generator.playing) { sleep_ms(2); }

    // sleep_ms(500);

    // set_rest_duration(0);

    /**
     * @brief Use this function to speed up or down your melodies.
     * Default tempo is 120bpm. Tempo does not affect tone().
     */
     // set_tempo(160);

     /**
      * @brief This is an example sound effect. Each note defines a pitch (float, in Hz)
      * and a duration (expressed in subdivisions of a whole note). This means that
      * a duration of 16 is half a duration of 8. Negative values represent dotted notation,
      * so that -8 = 8 + (8/2) = 12. This data structure is inspired by the work at
      * https://github.com/robsoncouto/arduino-songs/
      */
    note_t sfx[] = {
        {NOTE_C4, 16},
        {NOTE_C5, 32},
        {NOTE_C6, 64},
        {REST, 8}, // Pause at the end to space out repeats of the melody
        {MELODY_END, 0}, // Melody end code. Necessary to trigger repeats
    };

    note_t vader[] = {

        // Dart Vader theme (Imperial March) - Star wars 
        // Score available at https://musescore.com/user/202909/scores/1141521
        // The tenor saxophone part was used

        NOTE_A4,-4, NOTE_A4,-4, NOTE_A4,16, NOTE_A4,16, NOTE_A4,16, NOTE_A4,16, NOTE_F4,8, REST,8,
        NOTE_A4,-4, NOTE_A4,-4, NOTE_A4,16, NOTE_A4,16, NOTE_A4,16, NOTE_A4,16, NOTE_F4,8, REST,8,
        NOTE_A4,4, NOTE_A4,4, NOTE_A4,4, NOTE_F4,-8, NOTE_C5,16,

        NOTE_A4,4, NOTE_F4,-8, NOTE_C5,16, NOTE_A4,2,//4
        NOTE_E5,4, NOTE_E5,4, NOTE_E5,4, NOTE_F5,-8, NOTE_C5,16,
        NOTE_A4,4, NOTE_F4,-8, NOTE_C5,16, NOTE_A4,2,

        NOTE_A5,4, NOTE_A4,-8, NOTE_A4,16, NOTE_A5,4, NOTE_GS5,-8, NOTE_G5,16, //7 
        NOTE_DS5,16, NOTE_D5,16, NOTE_DS5,8, REST,8, NOTE_A4,8, NOTE_DS5,4, NOTE_D5,-8, NOTE_CS5,16,

        NOTE_C5,16, NOTE_B4,16, NOTE_C5,16, REST,8, NOTE_F4,8, NOTE_GS4,4, NOTE_F4,-8, NOTE_A4,-16,//9
        NOTE_C5,4, NOTE_A4,-8, NOTE_C5,16, NOTE_E5,2,

        NOTE_A5,4, NOTE_A4,-8, NOTE_A4,16, NOTE_A5,4, NOTE_GS5,-8, NOTE_G5,16, //7 
        NOTE_DS5,16, NOTE_D5,16, NOTE_DS5,8, REST,8, NOTE_A4,8, NOTE_DS5,4, NOTE_D5,-8, NOTE_CS5,16,

        NOTE_C5,16, NOTE_B4,16, NOTE_C5,16, REST,8, NOTE_F4,8, NOTE_GS4,4, NOTE_F4,-8, NOTE_A4,-16,//9
        NOTE_A4,4, NOTE_F4,-8, NOTE_C5,16, NOTE_A4,2,

    };


    /**
     * @brief Let's play the sfx we just defined, repeating it twice
     */
     // melody(&generator, vader, 2);

    engine.lcd_spi_bklt = LCD_BKLT;
    engine.lcd_spi_clk = LCD_SPI_CLK;
    engine.lcd_spi_cs = LCD_CS;
    engine.lcd_spi_dc = LCD_DC;
    engine.lcd_spi_instance = LCD_SPI_PORT;
    engine.lcd_spi_miso = LCD_SPI_MISO;
    engine.lcd_spi_mosi = LCD_SPI_MOSI;
    engine.lcd_spi_speed = LCD_SPI_SPEED;
    engine.lcd_spi_rst = LCD_RST;
    engine.display_w = 640UL; // surprise UL is needed to define a unsigned long wich is a uint32_t; even if u implicit cast the compiler will not recognize the constant and cast it from a int to a uint32_t cauzing overflow.
    engine.display_h = 480UL;
    engine.pre_init_clbk = pre_init;
    engine.post_init_clbk = post_init;
    engine.update_clbk = update;
    engine.render_clbk = render;
    engine.loop_clbk = loop_clbk;

    tinyengine_init(&engine);

    // prepare the framebuffer dimenstions here.

    te_fb_init(&frame_buf, 320UL, 240UL, true);


    printf("Free Memory %d", getFreeHeap());

    te_fb_clear(&frame_buf, 0x00);
    // tinyengine_init_renderer(&engine, TINYENGINE_RENDERER_LCD_ST7735R);
    tinyengine_init_renderer(&engine, TINYENGINE_RENDERER_DVI, &frame_buf);
    // tinyengine_init_renderer(&engine, TINYENGINE_RENDERER_LCD_ILI9488);

    tinyengine_start(&engine);
    tinyengine_start_loop(&engine);
    te_fb_destroy(&frame_buf);
}