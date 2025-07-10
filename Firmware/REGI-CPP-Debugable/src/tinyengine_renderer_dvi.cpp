#include "tinyengine_renderer_dvi.h"
#include <stdio.h>
#include "memory.h"
#include <malloc.h>
#include "pico/stdlib.h"
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

#define AURORA_1X_HEIGHT 1
#define AURORA_1X_WIDTH 256

static const uint32_t aurora_1x[]  = {
  0x000000, 0x111111, 0x222222, 0x333333, 0x444444, 0x555555, 0x666666, 0x777777, 0x888888, 0x999999, 0xaaaaaa, 0xbbbbbb, 0xcccccc, 0xdddddd, 0xeeeeee, 0xffffff, 0x7f7f00, 0xbfbf3f, 0xffff00, 0xffffbf, 0xff8181, 0xff0000, 0xbf3f3f, 0x7f0000, 0x500f0f, 0x7f007f, 0xbf3fbf, 0xf500f5, 0xff81fd, 0xcbc0ff, 0x8181ff, 0x0000ff, 0x3f3fbf, 0x00007f, 0x141455, 0x003f7f, 0x3f7fbf, 0x007fff, 0x81bfff, 0xbfffff, 0x00ffff, 0x3fbfbf, 0x007f7f, 0x007f00, 0x3fbf3f, 0x00ff00, 0xafffaf, 0xffbf00, 0xff7f00, 0xc87d4b, 0xc0afbc, 0x89aacb, 0x90a0a6, 0x94947e, 0x87826e, 0x606e7e, 0x5f69a0, 0x7278c0, 0x748ad0, 0x7d9be1, 0x8caaeb, 0x9bb9f5, 0xafc8f6, 0xd2e1f5, 0xff007f, 0x3b3b57, 0x3c4173, 0x55558e, 0x7373ab, 0x8f8fc7, 0xababe3, 0xdad2f8, 0xabc7e3, 0x739ec4, 0x57738f, 0x3b5773, 0x1f2d3b, 0x234141, 0x3b7373, 0x578f8f, 0x55a2a2, 0x72b5b5, 0x8fc7c7, 0xabdada, 0xc7eded, 0xabe3c7, 0x8fc7ab, 0x55be8e, 0x578f73, 0x3e7d58, 0x325046, 0x0f1e19, 0x375023, 0x3b573b, 0x506450, 0x49733b, 0x578f57, 0x73ab73, 0x82c064, 0x8fc78f, 0xa2d8a2, 0xfaf8e1, 0xcaeeb4, 0xc5e3ab, 0x8eb487, 0x5f7d50, 0x46690f, 0x232d1e, 0x464123, 0x73733b, 0xabab64, 0xc7c78f, 0xe3e3ab, 0xf1f1c7, 0xf0d2be, 0xe3c7ab, 0xdcb9a8, 0xc7ab8f, 0xc78f57, 0x8f7357, 0x73573b, 0x2d190f, 0x3b1f1f, 0x573b3b, 0x734949, 0x8f5757, 0xaa6e73, 0xca7676, 0xc78f8f, 0xe3abab, 0xf8dad0, 0xffe3e3, 0xc78fab, 0xc7578f, 0x8f5773, 0x733b57, 0x3c233c, 0x463246, 0x724072, 0x8f578f, 0xab57ab, 0xab73ab, 0xe1aceb, 0xf5dcff, 0xe3c7e3, 0xd2b9e1, 0xbea0d7, 0xb98fc7, 0xa07dc8, 0x915ac3, 0x37284b, 0x231632, 0x1e0a28, 0x111840, 0x001862, 0x0a14a5, 0x1020da, 0x4a52d5, 0x0a3cff, 0x325af5, 0x6262ff, 0x31bdf6, 0x3ca5ff, 0x0f9bd7, 0x0a6eda, 0x005ab4, 0x054ba0, 0x14325f, 0x0a5053, 0x006262, 0x5a808c, 0x0094ac, 0x0ab1b1, 0x5ad5e6, 0x10d5ff, 0x4aeaff, 0x41ffc8, 0x46f09b, 0x19dc96, 0x05c873, 0x05a86a, 0x146e3c, 0x053428, 0x084620, 0x0c5c0c, 0x059614, 0x0ad70a, 0x0ae614, 0x73ff7d, 0x5af04b, 0x14c500, 0x50b405, 0x4e8c1c, 0x323812, 0x809812, 0x91c406, 0x6ade00, 0xa8eb2d, 0xa5fe3c, 0xcdff6a, 0xffeb91, 0xffe655, 0xf0d77d, 0xd5de08, 0xde9c10, 0x5c5a05, 0x522c16, 0x7d370f, 0x9c4a00, 0x966432, 0xf65200, 0xbd6a18, 0xdc7823, 0xc39d69, 0xffa44a, 0xffb090, 0xffc55a, 0xfab9be, 0xf06e78, 0xff5a4a, 0xf64162, 0xf53c3c, 0xda1c10, 0xbd1000, 0x941023, 0x48210c, 0xb01050, 0xd01060, 0xd23287, 0xff419c, 0xff62bd, 0xff91b9, 0xffa5d7, 0xfac3d7, 0xfcc6f8, 0xff73e6, 0xff52ff, 0xe020da, 0xff29bd, 0xc510bd, 0xbe148c, 0x7b185a, 0x641464, 0x620041, 0x460a32, 0x371955, 0x8219a0, 0x7800c8, 0xbf50ff, 0xc56aff, 0xb9a0fa, 0x8c3afc, 0x781ee6, 0x3910bd, 0x4d3498, 0x371491
};


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


static TinyEngineRendererDVI* display = nullptr;

// ----------------------------------------------------------------------------
// DMA logic

void __scratch_x("display") dma_irq_handler() {
    display->gfx_dma_handler();
}

#define DMACH_PING 0
#define DMACH_PONG 1
// void __not_in_flash_func(dma_irq_handler()) {

void __scratch_x("display") TinyEngineRendererDVI::gfx_dma_handler() {
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
            uint8_t* src_ptr = &m_frame_buf.pixel_buffer_display[y * (640 >> h_repeat_shift)];
            // printf("%d\r\n", y * (640 >> h_repeat_shift));
            // for (int i = 0; i < timing_mode->h_active_pixels; i += 2) {
            for (int i = 0; i < 640; i += 2) {
                uint32_t val = display_palette[*src_ptr++];
                // uint32_t val = aurora_1x[*src_ptr++];
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


static __force_inline uint16_t colour_rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint16_t)r & 0xf8) >> 3 | ((uint16_t)g & 0xfc) << 3 | ((uint16_t)b & 0xf8) << 8;
}

static __force_inline uint8_t colour_rgb332(uint8_t r, uint8_t g, uint8_t b) {
    return (r & 0xc0) >> 6 | (g & 0xe0) >> 3 | (b & 0xe0) >> 0;
}

void scroll_framebuffer(void);

// void tinyengine_renderer_dvi_init(uint8_t* _display_buffer, uint32_t size, uint32_t width, uint32_t height) {
tinyengine_status_t TinyEngineRendererDVI::tinyengine_renderer_init() {
    //framebuffer->flip_blocking = te_renderer_dvi_wait_vsync_blocking;// set this in the main initiator in tinyengine.c
    display = this;
    // dma_claim_mask((1 << NUM_CHANS) - 1);
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
    // RGB 888 mode.
    // frame_bytes_per_pixel = 4;
    line_bytes_per_pixel = 4;

    // frame_buffer_display = (uint8_t*)calloc(frame_width * frame_height * frame_bytes_per_pixel, 1);
    // frame_buffer_back = (uint8_t*)calloc(frame_width * frame_height * frame_bytes_per_pixel, 1);


    // if (!framebuffer->is_dual) {
    //     teerr("Frame buffer needs to be dual for DVI.");

    //     return TINYENGINE_ERROR;
    // }

    // I just assign the pointers here because this makes the updating simpler.
    // s_frame_buf = framebuffer;
    // display_buf = _display_buffer;
    frame_buffer_display = m_frame_buf.pixel_buffer_display;
    // frame_buffer_back = framebuffer->pixel_buffer_back;
    // frame_buffer_display = framebuffer->pixel_buffer_display;
    // memset(frame_buffer_display, (uint8_t)(0xF0), frame_width * frame_height * frame_bytes_per_pixel);
    // memset(frame_buffer_back, (uint8_t)0xF0, frame_width * frame_height * frame_bytes_per_pixel);

    // if (m_frame_buf.m_pixel_buffer_size != frame_width * frame_height * frame_bytes_per_pixel) {
    //     // teerr("Frame buffer size Error, check size. Must be 320x240x1 bytes");
    //     return;
    // }

    memset(display_palette, 0, PALETTE_SIZE * sizeof(RGB888));
    for (uint8_t i = 0; i < 255; i++) {
        uint8_t r = (i >> 6) * 255 / 3;
        uint8_t g = ((i >> 2) & 7) * 255 / 7;
        uint8_t b = (i & 3) * 255 / 3;
        display_palette[i] = (r << 16) | (g << 8) | b;
    }

    // display_palette[255] = 0xFFFFFF; // just need to set 0xff to white this pallette method is clever. i wonder if i can switch pallettes based on sprites.

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


TinyEngineRendererDVI::TinyEngineRendererDVI(TinyEngineFrameBuffer& fr_buf) : m_frame_buf(fr_buf) {
    // Always use the bottom channels
    dma_claim_mask((1 << NUM_CHANS) - 1);
    tinyengine_renderer_init();
};

tinyengine_status_t TinyEngineRendererDVI::wait_for_vsync() {
    while (v_scanline >= MODE_V_FRONT_PORCH) __wfe();

    return TINYENGINE_OK;
}
