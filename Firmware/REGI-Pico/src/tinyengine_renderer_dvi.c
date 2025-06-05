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
// #include "mountains_640x480_rgb332.h.bak"
// #include "chicken_jockey.h"
uint8_t* framebuf;// = Chicken_Jockey;
// #define framebuf mountains_640x480

void tinyengine_renderer_dvi_set_buffer(uint8_t* buffer, uint32_t len) {
    framebuf = buffer;
}

// ----------------------------------------------------------------------------
// DVI constants

#define TMDS_CTRL_00 0x354u
#define TMDS_CTRL_01 0x0abu
#define TMDS_CTRL_10 0x154u
#define TMDS_CTRL_11 0x2abu

#define SYNC_V0_H0 (TMDS_CTRL_00 | (TMDS_CTRL_00 << 10) | (TMDS_CTRL_00 << 20))
#define SYNC_V0_H1 (TMDS_CTRL_01 | (TMDS_CTRL_00 << 10) | (TMDS_CTRL_00 << 20))
#define SYNC_V1_H0 (TMDS_CTRL_10 | (TMDS_CTRL_00 << 10) | (TMDS_CTRL_00 << 20))
#define SYNC_V1_H1 (TMDS_CTRL_11 | (TMDS_CTRL_00 << 10) | (TMDS_CTRL_00 << 20))

#define MODE_H_SYNC_POLARITY 0
#define MODE_H_FRONT_PORCH   16//16
#define MODE_H_SYNC_WIDTH    64//96
#define MODE_H_BACK_PORCH    120//48
#define MODE_H_ACTIVE_PIXELS 640 //  640

#define MODE_V_SYNC_POLARITY 0//0
#define MODE_V_FRONT_PORCH   1//10
#define MODE_V_SYNC_WIDTH    3//2
#define MODE_V_BACK_PORCH    16//33
#define MODE_V_ACTIVE_LINES 480 // 480

#define MODE_H_TOTAL_PIXELS ( \
    MODE_H_FRONT_PORCH + MODE_H_SYNC_WIDTH + \
    MODE_H_BACK_PORCH  + MODE_H_ACTIVE_PIXELS \
)
#define MODE_V_TOTAL_LINES  ( \
    MODE_V_FRONT_PORCH + MODE_V_SYNC_WIDTH + \
    MODE_V_BACK_PORCH  + MODE_V_ACTIVE_LINES \
)

#define HSTX_CMD_RAW         (0x0u << 12)
#define HSTX_CMD_RAW_REPEAT  (0x1u << 12)
#define HSTX_CMD_TMDS        (0x2u << 12)
#define HSTX_CMD_TMDS_REPEAT (0x3u << 12)
#define HSTX_CMD_NOP         (0xfu << 12)

// ----------------------------------------------------------------------------
// HSTX command lists

// Lists are padded with NOPs to be >= HSTX FIFO size, to avoid DMA rapidly
// pingponging and tripping up the IRQs.

static uint32_t vblank_line_vsync_off[] = {
    HSTX_CMD_RAW_REPEAT | MODE_H_FRONT_PORCH,
    SYNC_V1_H1,
    HSTX_CMD_RAW_REPEAT | MODE_H_SYNC_WIDTH,
    SYNC_V1_H0,
    HSTX_CMD_RAW_REPEAT | (MODE_H_BACK_PORCH + MODE_H_ACTIVE_PIXELS),
    SYNC_V1_H1,
    HSTX_CMD_NOP
};

static uint32_t vblank_line_vsync_on[] = {
    HSTX_CMD_RAW_REPEAT | MODE_H_FRONT_PORCH,
    SYNC_V0_H1,
    HSTX_CMD_RAW_REPEAT | MODE_H_SYNC_WIDTH,
    SYNC_V0_H0,
    HSTX_CMD_RAW_REPEAT | (MODE_H_BACK_PORCH + MODE_H_ACTIVE_PIXELS),
    SYNC_V0_H1,
    HSTX_CMD_NOP
};

static uint32_t vactive_line[] = {
    HSTX_CMD_RAW_REPEAT | MODE_H_FRONT_PORCH,
    SYNC_V1_H1,
    HSTX_CMD_NOP,
    HSTX_CMD_RAW_REPEAT | MODE_H_SYNC_WIDTH,
    SYNC_V1_H0,
    HSTX_CMD_NOP,
    HSTX_CMD_RAW_REPEAT | MODE_H_BACK_PORCH,
    SYNC_V1_H1,
    HSTX_CMD_TMDS | MODE_H_ACTIVE_PIXELS
};


te_fb_handle_t fra = { 0 };

// ----------------------------------------------------------------------------
// DMA logic

#define DMACH_PING 0
#define DMACH_PONG 1

// First we ping. Then we pong. Then... we ping again.
static bool dma_pong = false;

// A ping and a pong are cued up initially, so the first time we enter this
// handler it is to cue up the second ping after the first ping has completed.
// This is the third scanline overall (-> =2 because zero-based).
static uint v_scanline = 2;

// During the vertical active period, we take two IRQs per scanline: one to
// post the command list, and another to post the pixels.
static bool vactive_cmdlist_posted = false;

void __not_in_flash_func(dma_irq_handler()) {

    // dma_pong indicates the channel that just finished, which is the one
    // we're about to reload.
    uint ch_num = dma_pong ? DMACH_PONG : DMACH_PING;
    dma_channel_hw_t* ch = &dma_hw->ch[ch_num];
    dma_hw->intr = 1u << ch_num;
    dma_pong = !dma_pong;

    if (v_scanline >= MODE_V_FRONT_PORCH && v_scanline < (MODE_V_FRONT_PORCH + MODE_V_SYNC_WIDTH)) {
        ch->read_addr = (uintptr_t)vblank_line_vsync_on;
        ch->transfer_count = count_of(vblank_line_vsync_on);
    }
    else if (v_scanline < MODE_V_FRONT_PORCH + MODE_V_SYNC_WIDTH + MODE_V_BACK_PORCH) {
        ch->read_addr = (uintptr_t)vblank_line_vsync_off;
        ch->transfer_count = count_of(vblank_line_vsync_off);
    }
    else if (!vactive_cmdlist_posted) {
        ch->read_addr = (uintptr_t)vactive_line;
        ch->transfer_count = count_of(vactive_line);
        vactive_cmdlist_posted = true;
    }
    else {
        ch->read_addr = (uintptr_t)&framebuf[(v_scanline - (MODE_V_TOTAL_LINES - MODE_V_ACTIVE_LINES)) * MODE_H_ACTIVE_PIXELS];
        ch->transfer_count = MODE_H_ACTIVE_PIXELS / sizeof(uint32_t);
        vactive_cmdlist_posted = false;
    }

    if (!vactive_cmdlist_posted) {
        v_scanline = (v_scanline + 1) % MODE_V_TOTAL_LINES;
    }

    // if (++v_scanline == MODE_V_FRONT_PORCH + MODE_V_SYNC_WIDTH + MODE_V_BACK_PORCH + MODE_V_ACTIVE_LINES) {
    //     v_scanline = 0;
    //     // line_num = -1;
    //     // if (flip_next) {
    //     //     flip_next = false;
    //     //     display->flip_now();
    //     // }
    //     __sev();
    // }



}
// ----------------------------------------------------------------------------
// Main program

void __not_in_flash_func(wait_for_vsync()) {
    while (v_scanline >= MODE_V_FRONT_PORCH) __wfe();
}

static __force_inline uint16_t colour_rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint16_t)r & 0xf8) >> 3 | ((uint16_t)g & 0xfc) << 3 | ((uint16_t)b & 0xf8) << 8;
}

static __force_inline uint8_t colour_rgb332(uint8_t r, uint8_t g, uint8_t b) {
    return (r & 0xc0) >> 6 | (g & 0xe0) >> 3 | (b & 0xe0) >> 0;
}

void scroll_framebuffer(void);

uint32_t getTotalHeap(void) {
    extern char __StackLimit, __bss_end__;

    return &__StackLimit - &__bss_end__;
}

uint32_t getFreeHeap(void) {
    struct mallinfo m = mallinfo();

    return getTotalHeap() - m.uordblks;
}

uint32_t core1stack[128];
void __not_in_flash_func(HDMICore)(void) {

    // framebuf = fra.pixel_buffer;

    // telog("Frame Buffer Allocated; Size: %d Bytes", fb_handle->display_h * fb_handle->display_w * 1);
    // Ensure HSTX FIFO is clear
    // reset_block_num(RESET_HSTX);
    // sleep_us(10);
    // unreset_block_num_wait_blocking(RESET_HSTX);
    // sleep_us(10);
    // memset(framebuf, 0xE0, 480 * 640 * 1);

    // te_fb_clear(&fra, 0xE0);
    // te_fb_clear(&fra1, 0xC0);


    // Configure HSTX's TMDS encoder for RGB332
    hstx_ctrl_hw->expand_tmds =
        2 << HSTX_CTRL_EXPAND_TMDS_L2_NBITS_LSB |
        0 << HSTX_CTRL_EXPAND_TMDS_L2_ROT_LSB |
        2 << HSTX_CTRL_EXPAND_TMDS_L1_NBITS_LSB |
        29 << HSTX_CTRL_EXPAND_TMDS_L1_ROT_LSB |
        1 << HSTX_CTRL_EXPAND_TMDS_L0_NBITS_LSB |
        26 << HSTX_CTRL_EXPAND_TMDS_L0_ROT_LSB;

    // Pixels (TMDS) come in 4 8-bit chunks. Control symbols (RAW) are an
    // entire 32-bit word.
    hstx_ctrl_hw->expand_shift =
        4 << HSTX_CTRL_EXPAND_SHIFT_ENC_N_SHIFTS_LSB |
        8 << HSTX_CTRL_EXPAND_SHIFT_ENC_SHIFT_LSB |
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

    // Note we are leaving the HSTX clock at the SDK default of 125 MHz; since
    // we shift out two bits per HSTX clock cycle, this gives us an output of
    // 250 Mbps, which is very close to the bit clock for 480p 60Hz (252 MHz).
    // If we want the exact rate then we'll have to reconfigure PLLs.

    // HSTX outputs 0 through 7 appear on GPIO 12 through 19.
    // Pinout on Pico DVI sock:
    //
    //   GP12 D0+  GP13 D0-
    //   GP14 CK+  GP15 CK-
    //   GP16 D2+  GP17 D2-
    //   GP18 D1+  GP19 D1-

    // Assign clock pair to two neighbouring pins:
    // hstx_ctrl_hw->bit[2] = HSTX_CTRL_BIT0_CLK_BITS;
    // hstx_ctrl_hw->bit[3] = HSTX_CTRL_BIT0_CLK_BITS | HSTX_CTRL_BIT0_INV_BITS;
    // for (uint lane = 0; lane < 3; ++lane) {
    //     // For each TMDS lane, assign it to the correct GPIO pair based on the
    //     // desired pinout:
    //     static const int lane_to_output_bit[3] = { 0, 6, 4 };
    //     int bit = lane_to_output_bit[lane];
    //     // Output even bits during first half of each HSTX cycle, and odd bits
    //     // during second half. The shifter advances by two bits each cycle.
    //     uint32_t lane_data_sel_bits =
    //         (lane * 10) << HSTX_CTRL_BIT0_SEL_P_LSB |
    //         (lane * 10 + 1) << HSTX_CTRL_BIT0_SEL_N_LSB;
    //     // The two halves of each pair get identical data, but one pin is inverted.
    //     hstx_ctrl_hw->bit[bit] = lane_data_sel_bits;
    //     hstx_ctrl_hw->bit[bit + 1] = lane_data_sel_bits | HSTX_CTRL_BIT0_INV_BITS;
    // }


    // HSTX outputs 0 through 7 appear on GPIO 12 through 19.
    const int HSTX_FIRST_PIN = 12;
    int clk_p = 12;
    int rgb_p[3] = { 14, 16, 18 };
    // Assign clock pair to two neighbouring pins:
    hstx_ctrl_hw->bit[(clk_p)-HSTX_FIRST_PIN] = HSTX_CTRL_BIT0_CLK_BITS;
    hstx_ctrl_hw->bit[(clk_p ^ 1) - HSTX_FIRST_PIN] = HSTX_CTRL_BIT0_CLK_BITS | HSTX_CTRL_BIT0_INV_BITS;
    for (uint lane = 0; lane < 3; ++lane) {
        // For each TMDS lane, assign it to the correct GPIO pair based on the
        // desired pinout:
        int bit = rgb_p[lane];
        // Output even bits during first half of each HSTX cycle, and odd bits
        // during second half. The shifter advances by two bits each cycle.
        uint32_t lane_data_sel_bits =
            (lane * 10) << HSTX_CTRL_BIT0_SEL_P_LSB |
            (lane * 10 + 1) << HSTX_CTRL_BIT0_SEL_N_LSB;
        // The two halves of each pair get identical data, but one pin is inverted.
        hstx_ctrl_hw->bit[(bit)-HSTX_FIRST_PIN] = lane_data_sel_bits;
        hstx_ctrl_hw->bit[(bit ^ 1) - HSTX_FIRST_PIN] = lane_data_sel_bits | HSTX_CTRL_BIT0_INV_BITS;
    }

    for (int i = 12; i <= 19; ++i) {
        gpio_set_function(i, GPIO_FUNC_HSTX);
        gpio_set_drive_strength(i, GPIO_DRIVE_STRENGTH_4MA);
    }

    // Both channels are set up identically, to transfer a whole scanline and
    // then chain to the opposite channel. Each time a channel finishes, we
    // reconfigure the one that just finished, meanwhile the opposite channel
    // is already making progress.
    dma_channel_config c;
    c = dma_channel_get_default_config(DMACH_PING);
    channel_config_set_chain_to(&c, DMACH_PONG);
    channel_config_set_dreq(&c, DREQ_HSTX);
    dma_channel_configure(
        DMACH_PING,
        &c,
        &hstx_fifo_hw->fifo,
        vblank_line_vsync_off,
        count_of(vblank_line_vsync_off),
        false
    );
    c = dma_channel_get_default_config(DMACH_PONG);
    channel_config_set_chain_to(&c, DMACH_PING);
    channel_config_set_dreq(&c, DREQ_HSTX);
    dma_channel_configure(
        DMACH_PONG,
        &c,
        &hstx_fifo_hw->fifo,
        vblank_line_vsync_off,
        count_of(vblank_line_vsync_off),
        false
    );

    dma_hw->ints0 = (1u << DMACH_PING) | (1u << DMACH_PONG);
    dma_hw->inte0 = (1u << DMACH_PING) | (1u << DMACH_PONG);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_irq_handler);
    irq_set_enabled(DMA_IRQ_0, true);

    // bus_ctrl_hw->priority = BUSCTRL_BUS_PRIORITY_DMA_W_BITS | BUSCTRL_BUS_PRIORITY_DMA_R_BITS;
    bus_ctrl_hw->priority = 1;
    dma_channel_start(DMACH_PING);


    while (1) {
        __asm__("nop");
    }

}

tinyengine_status_t tinyengine_renderer_dvi_init(tinyengine_handle_t* engine_handle, uint8_t* pixel_buffer, uint32_t size) {

    // framebuf = calloc(640 * 480, 1  );
    framebuf = pixel_buffer;
    // te_fb_handle_t fra = { 0 };
    te_fb_handle_t fra1 = { 0 };


    tinyengine_handle_t eng = { 0 };

    eng.display_h = 480UL;
    eng.display_w = 640UL;

    // te_fb_init(&fra, &eng);

    // te_fb_init(&fra1, &eng);

    // framebuf = fra.pixel_buffer;

    // // telog("Frame Buffer Allocated; Size: %d Bytes", fb_handle->display_h * fb_handle->display_w * 1);
    // // Ensure HSTX FIFO is clear
    // // reset_block_num(RESET_HSTX);
    // // sleep_us(10);
    // // unreset_block_num_wait_blocking(RESET_HSTX);
    // // sleep_us(10);
    // // memset(framebuf, 0xE0, 480 * 640 * 1);

    // te_fb_clear(&fra, 0xE0);
    // te_fb_clear(&fra1, 0xC0);


    // // Configure HSTX's TMDS encoder for RGB332
    // hstx_ctrl_hw->expand_tmds =
    //     2 << HSTX_CTRL_EXPAND_TMDS_L2_NBITS_LSB |
    //     0 << HSTX_CTRL_EXPAND_TMDS_L2_ROT_LSB |
    //     2 << HSTX_CTRL_EXPAND_TMDS_L1_NBITS_LSB |
    //     29 << HSTX_CTRL_EXPAND_TMDS_L1_ROT_LSB |
    //     1 << HSTX_CTRL_EXPAND_TMDS_L0_NBITS_LSB |
    //     26 << HSTX_CTRL_EXPAND_TMDS_L0_ROT_LSB;

    // // Pixels (TMDS) come in 4 8-bit chunks. Control symbols (RAW) are an
    // // entire 32-bit word.
    // hstx_ctrl_hw->expand_shift =
    //     4 << HSTX_CTRL_EXPAND_SHIFT_ENC_N_SHIFTS_LSB |
    //     8 << HSTX_CTRL_EXPAND_SHIFT_ENC_SHIFT_LSB |
    //     1 << HSTX_CTRL_EXPAND_SHIFT_RAW_N_SHIFTS_LSB |
    //     0 << HSTX_CTRL_EXPAND_SHIFT_RAW_SHIFT_LSB;

    // // Serial output config: clock period of 5 cycles, pop from command
    // // expander every 5 cycles, shift the output shiftreg by 2 every cycle.
    // hstx_ctrl_hw->csr = 0;
    // hstx_ctrl_hw->csr =
    //     HSTX_CTRL_CSR_EXPAND_EN_BITS |
    //     5u << HSTX_CTRL_CSR_CLKDIV_LSB |
    //     5u << HSTX_CTRL_CSR_N_SHIFTS_LSB |
    //     2u << HSTX_CTRL_CSR_SHIFT_LSB |
    //     HSTX_CTRL_CSR_EN_BITS;

    // // Note we are leaving the HSTX clock at the SDK default of 125 MHz; since
    // // we shift out two bits per HSTX clock cycle, this gives us an output of
    // // 250 Mbps, which is very close to the bit clock for 480p 60Hz (252 MHz).
    // // If we want the exact rate then we'll have to reconfigure PLLs.

    // // HSTX outputs 0 through 7 appear on GPIO 12 through 19.
    // // Pinout on Pico DVI sock:
    // //
    // //   GP12 D0+  GP13 D0-
    // //   GP14 CK+  GP15 CK-
    // //   GP16 D2+  GP17 D2-
    // //   GP18 D1+  GP19 D1-

    // // Assign clock pair to two neighbouring pins:
    // hstx_ctrl_hw->bit[2] = HSTX_CTRL_BIT0_CLK_BITS;
    // hstx_ctrl_hw->bit[3] = HSTX_CTRL_BIT0_CLK_BITS | HSTX_CTRL_BIT0_INV_BITS;
    // for (uint lane = 0; lane < 3; ++lane) {
    //     // For each TMDS lane, assign it to the correct GPIO pair based on the
    //     // desired pinout:
    //     static const int lane_to_output_bit[3] = { 0, 6, 4 };
    //     int bit = lane_to_output_bit[lane];
    //     // Output even bits during first half of each HSTX cycle, and odd bits
    //     // during second half. The shifter advances by two bits each cycle.
    //     uint32_t lane_data_sel_bits =
    //         (lane * 10) << HSTX_CTRL_BIT0_SEL_P_LSB |
    //         (lane * 10 + 1) << HSTX_CTRL_BIT0_SEL_N_LSB;
    //     // The two halves of each pair get identical data, but one pin is inverted.
    //     hstx_ctrl_hw->bit[bit] = lane_data_sel_bits;
    //     hstx_ctrl_hw->bit[bit + 1] = lane_data_sel_bits | HSTX_CTRL_BIT0_INV_BITS;
    // }

    // for (int i = 12; i <= 19; ++i) {
    //     gpio_set_function(i, 0); // HSTX
    // }

    // // Both channels are set up identically, to transfer a whole scanline and
    // // then chain to the opposite channel. Each time a channel finishes, we
    // // reconfigure the one that just finished, meanwhile the opposite channel
    // // is already making progress.
    // dma_channel_config c;
    // c = dma_channel_get_default_config(DMACH_PING);
    // channel_config_set_chain_to(&c, DMACH_PONG);
    // channel_config_set_dreq(&c, DREQ_HSTX);
    // dma_channel_configure(
    //     DMACH_PING,
    //     &c,
    //     &hstx_fifo_hw->fifo,
    //     vblank_line_vsync_off,
    //     count_of(vblank_line_vsync_off),
    //     false
    // );
    // c = dma_channel_get_default_config(DMACH_PONG);
    // channel_config_set_chain_to(&c, DMACH_PING);
    // channel_config_set_dreq(&c, DREQ_HSTX);
    // dma_channel_configure(
    //     DMACH_PONG,
    //     &c,
    //     &hstx_fifo_hw->fifo,
    //     vblank_line_vsync_off,
    //     count_of(vblank_line_vsync_off),
    //     false
    // );

    // dma_hw->ints0 = (1u << DMACH_PING) | (1u << DMACH_PONG);
    // dma_hw->inte0 = (1u << DMACH_PING) | (1u << DMACH_PONG);
    // irq_set_exclusive_handler(DMA_IRQ_0, dma_irq_handler);
    // irq_set_enabled(DMA_IRQ_0, true);

    // bus_ctrl_hw->priority = BUSCTRL_BUS_PRIORITY_DMA_W_BITS | BUSCTRL_BUS_PRIORITY_DMA_R_BITS;

    // dma_channel_start(DMACH_PING);


    double counter = 0;
    double x1 = 10, y1 = 20, x2 = 0, y2 = 50;

    struct ball {

        double x, y;
        double dirx, diry;
        double spd;

    } ball1, ball2, ball3;

    struct ball balls[10];

    for (uint8_t i = 0; i < 10; i++) {
        balls[i].x = 2;
        balls[i].y = 10;
        balls[i].dirx = 0.1;
        balls[i].diry = 0.5;
        balls[i].spd = 500;
    }
    sleep_ms(100); // for some reason this is needed before core launch so that 
    multicore_launch_core1_with_stack(HDMICore, core1stack, 512);
    core1stack[0] = 0x12345678;

    // while (1) {
    //     te_fb_clear(&fra, 0x95);
    //     // fillScreen(ST7735_BLACK);
    //     for (uint8_t i = 0; i < 10; i++) {
    //         switch (i % 2)
    //         {
    //         case 1:
    //             te_fb_draw_filled_circle(&fra, balls[i].x, balls[i].y, 10, 0xBD);
    //             // fillCircle(balls[i].x, balls[i].y, 10, ST7735_GREEN);
    //             break;
    //         case 0:
    //             te_fb_draw_filled_circle(&fra, balls[i].x, balls[i].y, 10, 0xBD);
    //             // fillCircle(balls[i].x, balls[i].y, 10, ST7735_RED);
    //             break;
    //         default:
    //             break;
    //         }
    //     }

    //     for (uint8_t i = 0; i < 10; i++) {
    //         if (balls[i].x >= 640) {
    //             balls[i].dirx = -1 * ((double)get_rand_32() / (double)__RAND_MAX);
    //         }
    //         if (balls[i].y >= 480) {
    //             balls[i].diry = -1 * ((double)get_rand_32() / (double)__RAND_MAX);
    //         }
    //         if (balls[i].x <= 0) {
    //             balls[i].dirx = 1 * ((double)get_rand_32() / (double)__RAND_MAX);
    //         }
    //         if (balls[i].y <= 0) {
    //             balls[i].diry = 1 * ((double)get_rand_32() / (double)__RAND_MAX);
    //         }
    //         balls[i].x += balls[i].spd * 0.08 * balls[i].dirx;
    //         balls[i].y += balls[i].spd * 0.08 * balls[i].diry;
    //     }

    //     sleep_us(10);

    //     // telog("Free Heap %d Bytes", getFreeHeap());

    // }
    return TINYENGINE_OK;
}