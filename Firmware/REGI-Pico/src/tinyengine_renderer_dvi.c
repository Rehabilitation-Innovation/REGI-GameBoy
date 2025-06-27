#include "tinyengine_renderer_dvi.h"
#include <stdio.h>
#include "memory.h"
#include <malloc.h>
#include "pico/stdlib.h"
#include "tinyengine_framebuffer.h"
#include "hardware/dma.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/structs/bus_ctrl.h"
#include "hardware/structs/hstx_ctrl.h"
#include "hardware/structs/hstx_fifo.h"
#include "hardware/structs/sio.h"
#include "pico/multicore.h"
#include "pico/sem.h"
#include "pico/rand.h"
#include "hardware/pll.h"
#include "hardware/vreg.h"
#include "hardware/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/clocks.h"
#include "main.h"
#include "tinyengine.h"

#define MODE_H_SYNC_POLARITY 0
#define MODE_H_FRONT_PORCH   16
#define MODE_H_SYNC_WIDTH    96
#define MODE_H_BACK_PORCH    48
#define MODE_H_ACTIVE_PIXELS 640

#define MODE_V_SYNC_POLARITY 0
#define MODE_V_FRONT_PORCH   10
#define MODE_V_SYNC_WIDTH    2
#define MODE_V_BACK_PORCH    33
#define MODE_V_ACTIVE_LINES  480

#define TMDS_CTRL_00 0x354u
#define TMDS_CTRL_01 0x0abu
#define TMDS_CTRL_10 0x154u
#define TMDS_CTRL_11 0x2abu

#define TMDS_BALANCED_LOW  0x307u
#define TMDS_BALANCED_HIGH 0x2f0u

#define TMDS_BLACK_A 0x100u
#define TMDS_BLACK_B 0x1ffu

#define SYNC_V0_H0 (TMDS_CTRL_00 | (TMDS_CTRL_00 << 10) | (TMDS_CTRL_00 << 20))
#define SYNC_V0_H1 (TMDS_CTRL_01 | (TMDS_CTRL_00 << 10) | (TMDS_CTRL_00 << 20))
#define SYNC_V1_H0 (TMDS_CTRL_10 | (TMDS_CTRL_00 << 10) | (TMDS_CTRL_00 << 20))
#define SYNC_V1_H1 (TMDS_CTRL_11 | (TMDS_CTRL_00 << 10) | (TMDS_CTRL_00 << 20))
#define MISSING_PIXEL (TMDS_BALANCED_LOW | (TMDS_BALANCED_LOW << 10) | (TMDS_BALANCED_HIGH << 20))
#define BLACK_PIXEL (TMDS_BALANCED_LOW | (TMDS_BALANCED_LOW << 10) | (TMDS_BALANCED_LOW << 20))
#define BLACK_PIXEL_A (TMDS_BLACK_A | (TMDS_BLACK_A << 10) | (TMDS_BLACK_A << 20))
#define BLACK_PIXEL_B (TMDS_BLACK_B | (TMDS_BLACK_B << 10) | (TMDS_BLACK_B << 20))

#define HSTX_CMD_RAW         (0x0u << 12)
#define HSTX_CMD_RAW_REPEAT  (0x1u << 12)
#define HSTX_CMD_TMDS        (0x2u << 12)
#define HSTX_CMD_TMDS_REPEAT (0x3u << 12)
#define HSTX_CMD_NOP         (0xfu << 12)
// ----------------------------------------------------------------------------
// HSTX command lists

// Lists are padded with NOPs to be >= HSTX FIFO size, to avoid DMA rapidly
// pingponging and tripping up the IRQs.

static const uint32_t vblank_line_vsync_off_src[] = {
    HSTX_CMD_RAW_REPEAT,
    SYNC_V1_H1,
    HSTX_CMD_RAW_REPEAT,
    SYNC_V1_H0,
    HSTX_CMD_RAW_REPEAT,
    SYNC_V1_H1
};
static uint32_t vblank_line_vsync_off[count_of(vblank_line_vsync_off_src)];

static const uint32_t vblank_line_vsync_on_src[] = {
    HSTX_CMD_RAW_REPEAT,
    SYNC_V0_H1,
    HSTX_CMD_RAW_REPEAT,
    SYNC_V0_H0,
    HSTX_CMD_RAW_REPEAT,
    SYNC_V0_H1
};
static uint32_t vblank_line_vsync_on[count_of(vblank_line_vsync_on_src)];

static const uint32_t vactive_line_header_src[] = {
    HSTX_CMD_RAW_REPEAT,
    SYNC_V1_H1,
    HSTX_CMD_RAW_REPEAT,
    SYNC_V1_H0,
    HSTX_CMD_RAW_REPEAT,
    SYNC_V1_H1,
    HSTX_CMD_TMDS
};
static uint32_t vactive_line_header[count_of(vactive_line_header_src)];

static const uint32_t vactive_text_line_header_src[] = {
    HSTX_CMD_RAW_REPEAT,
    SYNC_V1_H1,
    HSTX_CMD_RAW_REPEAT,
    SYNC_V1_H0,
    HSTX_CMD_RAW_REPEAT,
    SYNC_V1_H1,
    HSTX_CMD_RAW | 6,
    BLACK_PIXEL_A,
    BLACK_PIXEL_B,
    BLACK_PIXEL_A,
    BLACK_PIXEL_B,
    BLACK_PIXEL_A,
    BLACK_PIXEL_B,
    HSTX_CMD_TMDS
};
static uint32_t vactive_text_line_header[count_of(vactive_text_line_header_src)];


// ----------------------------------------------------------------------------
// DMA logic

#define DMACH_PING 0
#define DMACH_PONG 1

static uint16_t display_width = 320;
static uint16_t display_height = 240;
static uint16_t frame_width = 320;
static uint16_t frame_height = 240;
static uint8_t frame_bytes_per_pixel = 2;
static uint8_t h_repeat = 4;
static uint8_t v_repeat = 4;
typedef uint32_t RGB888;

// DMA scanline filling
static uint ch_num = 0;
static int line_num = -1;

static volatile int v_scanline = 2;
static volatile bool flip_next;

static bool inited = false;

static uint32_t* line_buffers;
static int v_inactive_total;
static int v_total_active_lines;

static uint h_repeat_shift = 1;
static uint v_repeat_shift = 1;
static int line_bytes_per_pixel;

static uint8_t* frame_buffer_display;
static uint8_t* frame_buffer_back;

static te_fb_handle_t* s_frame_buf;

#define NUM_FRAME_LINES 2
#define NUM_CHANS 3
static int PALETTE_SIZE = 256;

static RGB888 display_palette[256];

// First we ping. Then we pong. Then... we ping again.
static bool dma_pong = false;

// A ping and a pong are cued up initially, so the first time we enter this
// handler it is to cue up the second ping after the first ping has completed.
// This is the third scanline overall (-> =2 because zero-based).
// static uint v_scanline = 2;

// During the vertical active period, we take two IRQs per scanline: one to
// post the command list, and another to post the pixels.
static bool vactive_cmdlist_posted = false;

void __not_in_flash_func(dma_irq_handler()) {

    // ch_num indicates the channel that just finished, which is the one
    // we're about to reload.
    dma_channel_hw_t* ch = &dma_hw->ch[ch_num];
    dma_hw->intr = 1u << ch_num;
    if (++ch_num == NUM_CHANS) ch_num = 0;

    // if (v_scanline >= timing_mode->v_front_porch && v_scanline < (timing_mode->v_front_porch + timing_mode->v_sync_width)) {
    if (v_scanline >= 10 && v_scanline < (10 + 2)) {
        ch->read_addr = (uintptr_t)vblank_line_vsync_on;
        ch->transfer_count = count_of(vblank_line_vsync_on);
    }
    else if (v_scanline < v_inactive_total) {
        ch->read_addr = (uintptr_t)vblank_line_vsync_off;
        ch->transfer_count = count_of(vblank_line_vsync_off);
    }
    else {
        const int y = (v_scanline - v_inactive_total) >> v_repeat_shift;
        const int new_line_num = (v_repeat_shift == 0) ? ch_num : (y & (NUM_FRAME_LINES - 1));
        // const uint line_buf_total_len = ((timing_mode->h_active_pixels * line_bytes_per_pixel) >> 2) + count_of(vactive_line_header);
        const uint line_buf_total_len = ((640 * line_bytes_per_pixel) >> 2) + count_of(vactive_line_header);

        ch->read_addr = (uintptr_t)&line_buffers[new_line_num * line_buf_total_len];
        ch->transfer_count = line_buf_total_len;

        // Fill line buffer
        if (line_num != new_line_num)
        {
            line_num = new_line_num;
            uint32_t* dst_ptr = &line_buffers[line_num * line_buf_total_len + count_of(vactive_line_header)];

            // uint8_t* src_ptr = &frame_buffer_display[y * (timing_mode->h_active_pixels >> h_repeat_shift)];
            uint8_t* src_ptr = &s_frame_buf->pixel_buffer_display[y * (640 >> h_repeat_shift)];
            // printf("%d\r\n", y * (640 >> h_repeat_shift));
            // for (int i = 0; i < timing_mode->h_active_pixels; i += 2) {
            for (int i = 0; i < 640; i += 2) {
                // uint32_t val = display_palette[*src_ptr++];
                uint32_t val = display_palette[*src_ptr++];
                // src_ptr += 1;
                *dst_ptr++ = val;//0xEB0000;
                *dst_ptr++ = val;//0xEB0000;
                // *src_ptr++;
            }


        }
    }

    if (++v_scanline == v_total_active_lines) {
        v_scanline = 0;
        line_num = -1;
        if (flip_next) {
            flip_next = false;
            // uint8_t* frame_buffer_temp = frame_buffer_back;
            // // display->flip_now();
            // // frame_buffer_back = frame_buffer_display;
            // // frame_buffer_display = frame_buffer_temp;
        }
        __sev();
    }
}

void __not_in_flash_func(te_renderer_dvi_wait_for_vsync()) {
    // while (v_scanline >= MODE_V_FRONT_PORCH) __wfe();
    while (v_scanline >= MODE_V_FRONT_PORCH) __wfe();
}


void __not_in_flash_func(te_renderer_dvi_wait_vsync_blocking()) {
    while (v_scanline >= MODE_V_FRONT_PORCH) {

        __wfe();
    }

    // flip_next = true;
    // while (flip_next) __wfe();
}

// just a wrapper; idk maybe in the future some race handling can be done here?
void __not_in_flash_func(tinyengine_renderer_dvi_flip_blocking()) {
    te_renderer_dvi_wait_vsync_blocking();

}

static __force_inline uint16_t colour_rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint16_t)r & 0xf8) >> 3 | ((uint16_t)g & 0xfc) << 3 | ((uint16_t)b & 0xf8) << 8;
}

static __force_inline uint8_t colour_rgb332(uint8_t r, uint8_t g, uint8_t b) {
    return (r & 0xc0) >> 6 | (g & 0xe0) >> 3 | (b & 0xe0) >> 0;
}

void scroll_framebuffer(void);

tinyengine_status_t tinyengine_renderer_dvi_init(tinyengine_handle_t* engine_handle, te_fb_handle_t* framebuffer) {

    //framebuffer->flip_blocking = te_renderer_dvi_wait_vsync_blocking;// set this in the main initiator in tinyengine.c

    dma_claim_mask((1 << NUM_CHANS) - 1);
    ch_num = 0;
    line_num = -1;
    v_scanline = 2;
    flip_next = false;

    // v_inactive_total = timing_mode->v_front_porch + timing_mode->v_sync_width + timing_mode->v_back_porch;
    v_inactive_total = MODE_V_FRONT_PORCH + MODE_V_SYNC_WIDTH + MODE_V_BACK_PORCH;
    // v_total_active_lines = v_inactive_total + timing_mode->v_active_lines;
    v_total_active_lines = v_inactive_total + MODE_V_ACTIVE_LINES;
    v_repeat = 1 << v_repeat_shift;
    h_repeat = 1 << h_repeat_shift;

    memcpy(vblank_line_vsync_off, vblank_line_vsync_off_src, sizeof(vblank_line_vsync_off_src));
    // vblank_line_vsync_off[0] |= timing_mode->h_front_porch;
    // vblank_line_vsync_off[2] |= timing_mode->h_sync_width;
    // vblank_line_vsync_off[4] |= timing_mode->h_back_porch + timing_mode->h_active_pixels;
    vblank_line_vsync_off[0] |= MODE_H_FRONT_PORCH;
    vblank_line_vsync_off[2] |= MODE_H_SYNC_WIDTH;
    vblank_line_vsync_off[4] |= MODE_H_BACK_PORCH + MODE_H_ACTIVE_PIXELS;

    memcpy(vblank_line_vsync_on, vblank_line_vsync_on_src, sizeof(vblank_line_vsync_on_src));
    // vblank_line_vsync_on[0] |= timing_mode->h_front_porch;
    // vblank_line_vsync_on[2] |= timing_mode->h_sync_width;
    // vblank_line_vsync_on[4] |= timing_mode->h_back_porch + timing_mode->h_active_pixels;
    vblank_line_vsync_on[0] |= MODE_H_FRONT_PORCH;
    vblank_line_vsync_on[2] |= MODE_H_SYNC_WIDTH;
    vblank_line_vsync_on[4] |= MODE_H_BACK_PORCH + MODE_H_ACTIVE_PIXELS;

    memcpy(vactive_line_header, vactive_line_header_src, sizeof(vactive_line_header_src));
    // vactive_line_header[0] |= timing_mode->h_front_porch;
    // vactive_line_header[2] |= timing_mode->h_sync_width;
    // vactive_line_header[4] |= timing_mode->h_back_porch;
    // vactive_line_header[6] |= timing_mode->h_active_pixels;
    vactive_line_header[0] |= MODE_H_FRONT_PORCH;
    vactive_line_header[2] |= MODE_H_SYNC_WIDTH;
    vactive_line_header[4] |= MODE_H_BACK_PORCH;
    vactive_line_header[6] |= MODE_H_ACTIVE_PIXELS;

    memcpy(vactive_text_line_header, vactive_text_line_header_src, sizeof(vactive_text_line_header_src));
    // vactive_text_line_header[0] |= timing_mode->h_front_porch;
    // vactive_text_line_header[2] |= timing_mode->h_sync_width;
    // vactive_text_line_header[4] |= timing_mode->h_back_porch;
    // vactive_text_line_header[7 + 6] |= timing_mode->h_active_pixels - 6;
    vactive_text_line_header[0] |= MODE_H_FRONT_PORCH;
    vactive_text_line_header[2] |= MODE_H_SYNC_WIDTH;
    vactive_text_line_header[4] |= MODE_H_BACK_PORCH;
    vactive_text_line_header[7 + 6] |= MODE_H_ACTIVE_PIXELS - 6;

    // RGB332 pallete mode
    frame_bytes_per_pixel = 1;
    line_bytes_per_pixel = 4;

    // frame_buffer_display = (uint8_t*)calloc(frame_width * frame_height * frame_bytes_per_pixel, 1);
    // frame_buffer_back = (uint8_t*)calloc(frame_width * frame_height * frame_bytes_per_pixel, 1);


    if (!framebuffer->is_dual) {
        teerr("Frame buffer needs to be dual for DVI.");

        return TINYENGINE_ERROR;
    }

    // I just assign the pointers here because this makes the updating simpler.
    s_frame_buf = framebuffer;
    // frame_buffer_back = framebuffer->pixel_buffer_back;
    // frame_buffer_display = framebuffer->pixel_buffer_display;
    // memset(frame_buffer_display, (uint8_t)(0xF0), frame_width * frame_height * frame_bytes_per_pixel);
    // memset(frame_buffer_back, (uint8_t)0xF0, frame_width * frame_height * frame_bytes_per_pixel);

    if (framebuffer->pixel_buffer_size != frame_width * frame_height * frame_bytes_per_pixel) {
        teerr("Frame buffer size Error, check size. Must be 320x240x1 bytes");
        return TINYENGINE_ERROR;
    }

    memset(display_palette, 0, PALETTE_SIZE * sizeof(RGB888));
    for (uint8_t i = 0; i < 255; i++) {
        uint8_t r = (i >> 6) * 255 / 3;
        uint8_t g = ((i >> 2) & 7) * 255 / 7;
        uint8_t b = (i & 3) * 255 / 3;
        display_palette[i] = (r << 16) | (g << 8) | b;
    }

    display_palette[255] = 0xFFFFFF; // just need to set 0xff to white this pallette method is clever. i wonder if i can switch pallettes based on sprites.

    // const bool is_text_mode = (mode == MODE_TEXT_MONO || mode == MODE_TEXT_RGB111);
    const int frame_pixel_words = (frame_width * h_repeat * line_bytes_per_pixel + 3) >> 2;
    // const int frame_line_words = frame_pixel_words + (is_text_mode ? count_of(vactive_text_line_header) : count_of(vactive_line_header));
    const int frame_line_words = frame_pixel_words + count_of(vactive_line_header);
    const int frame_lines = (v_repeat == 1) ? NUM_CHANS : NUM_FRAME_LINES;
    line_buffers = (uint32_t*)malloc(frame_line_words * 4 * frame_lines);

    for (int i = 0; i < frame_lines; ++i)
    {
        memcpy(&line_buffers[i * frame_line_words], vactive_line_header, count_of(vactive_line_header) * sizeof(uint32_t));
    }

    // Ensure HSTX FIFO is clear
    reset_block_num(RESET_HSTX);
    sleep_us(10);
    unreset_block_num_wait_blocking(RESET_HSTX);
    sleep_us(10);

    // Configure HSTX's TMDS encoder for RGB888
    hstx_ctrl_hw->expand_tmds =
        7 << HSTX_CTRL_EXPAND_TMDS_L2_NBITS_LSB |
        16 << HSTX_CTRL_EXPAND_TMDS_L2_ROT_LSB |
        7 << HSTX_CTRL_EXPAND_TMDS_L1_NBITS_LSB |
        8 << HSTX_CTRL_EXPAND_TMDS_L1_ROT_LSB |
        7 << HSTX_CTRL_EXPAND_TMDS_L0_NBITS_LSB |
        0 << HSTX_CTRL_EXPAND_TMDS_L0_ROT_LSB;

    // Pixels and control symbols (RAW) are an
    // entire 32-bit word.
    hstx_ctrl_hw->expand_shift =
        1 << HSTX_CTRL_EXPAND_SHIFT_ENC_N_SHIFTS_LSB |
        0 << HSTX_CTRL_EXPAND_SHIFT_ENC_SHIFT_LSB |
        1 << HSTX_CTRL_EXPAND_SHIFT_RAW_N_SHIFTS_LSB |
        0 << HSTX_CTRL_EXPAND_SHIFT_RAW_SHIFT_LSB;

    // Serial output config: clock period of 5 cycles, pop from command
        // expander every 5 cycles, shift the output shiftreg by 2 every cycle.
    hstx_ctrl_hw->csr = 0;
    hstx_ctrl_hw->csr =
        HSTX_CTRL_CSR_EXPAND_EN_BITS |
        5u << HSTX_CTRL_CSR_CLKDIV_LSB |
        5u << HSTX_CTRL_CSR_N_SHIFTS_LSB |
        2u << HSTX_CTRL_CSR_SHIFT_LSB |
        HSTX_CTRL_CSR_EN_BITS;


    // HSTX outputs 0 through 7 appear on GPIO 12 through 19.
    int HSTX_FIRST_PIN = 12;
    int clk_p = 12;
    int rgb_p[3] = { 14, 16, 18 };

    // Assign clock pair to two neighbouring pins:
    {
        int bit = clk_p - HSTX_FIRST_PIN;
        hstx_ctrl_hw->bit[bit] = HSTX_CTRL_BIT0_CLK_BITS;
        hstx_ctrl_hw->bit[bit ^ 1] = HSTX_CTRL_BIT0_CLK_BITS | HSTX_CTRL_BIT0_INV_BITS;
    }

    for (uint lane = 0; lane < 3; ++lane) {
        // For each TMDS lane, assign it to the correct GPIO pair based on the
        // desired pinout:
        int bit = rgb_p[lane] - HSTX_FIRST_PIN;
        // Output even bits during first half of each HSTX cycle, and odd bits
        // during second half. The shifter advances by two bits each cycle.
        uint32_t lane_data_sel_bits =
            (lane * 10) << HSTX_CTRL_BIT0_SEL_P_LSB |
            (lane * 10 + 1) << HSTX_CTRL_BIT0_SEL_N_LSB;
        // The two halves of each pair get identical data, but one pin is inverted.
        hstx_ctrl_hw->bit[bit] = lane_data_sel_bits;
        hstx_ctrl_hw->bit[bit ^ 1] = lane_data_sel_bits | HSTX_CTRL_BIT0_INV_BITS;
    }

    for (int i = 12; i <= 19; ++i) {
        gpio_set_function(i, GPIO_FUNC_HSTX);
        gpio_set_drive_strength(i, GPIO_DRIVE_STRENGTH_4MA);
        if (252000 > 900000) {
            gpio_set_slew_rate(i, GPIO_SLEW_RATE_FAST);
        }
    }

    printf("GPIO Done\r\n");

    // The channels are set up identically, to transfer a whole scanline and
        // then chain to the next channel. Each time a channel finishes, we
        // reconfigure the one that just finished, meanwhile the other channel(s)
        // are already making progress.
        // Using just 2 channels was insufficient to avoid issues with the IRQ.
    dma_channel_config c;
    c = dma_channel_get_default_config(0);
    channel_config_set_chain_to(&c, 1);
    channel_config_set_dreq(&c, DREQ_HSTX);
    dma_channel_configure(
        0,
        &c,
        &hstx_fifo_hw->fifo,
        vblank_line_vsync_off,
        count_of(vblank_line_vsync_off),
        false
    );
    c = dma_channel_get_default_config(1);
    channel_config_set_chain_to(&c, 2);
    channel_config_set_dreq(&c, DREQ_HSTX);
    dma_channel_configure(
        1,
        &c,
        &hstx_fifo_hw->fifo,
        vblank_line_vsync_off,
        count_of(vblank_line_vsync_off),
        false
    );
    for (int i = 2; i < NUM_CHANS; ++i) {
        c = dma_channel_get_default_config(i);
        channel_config_set_chain_to(&c, (i + 1) % NUM_CHANS);
        channel_config_set_dreq(&c, DREQ_HSTX);
        dma_channel_configure(
            i,
            &c,
            &hstx_fifo_hw->fifo,
            vblank_line_vsync_off,
            count_of(vblank_line_vsync_off),
            false
        );
    }

    printf("DMA channels claimed\r\n");

    dma_hw->intr = (1 << NUM_CHANS) - 1;
    dma_hw->ints2 = (1 << NUM_CHANS) - 1;
    dma_hw->inte2 = (1 << NUM_CHANS) - 1;

    irq_set_exclusive_handler(DMA_IRQ_2, dma_irq_handler);
    irq_set_enabled(DMA_IRQ_2, true);

    dma_channel_start(0);

    printf("DVHSTX started\r\n");

    return TINYENGINE_OK;
}