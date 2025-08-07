#include "main.h" 

#include <stdio.h>

#include "hardware/spi.h"

#include "pico/stdlib.h"

#include "pico/rand.h"
#include "tinyengine.h"

#include "hardware/pll.h"
#include "hardware/vreg.h"
#include "hardware/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/clocks.h"
#include "game.h"

#include "hardware/dma.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/structs/bus_ctrl.h"
#include "hardware/structs/hstx_ctrl.h"
#include "hardware/structs/hstx_fifo.h"
#include "hardware/structs/sio.h"
#include "pico/multicore.h"
#include "pico/sem.h"
void measure_freqs(void);

void isr_hardfault(void) {
    printf("Hardfault\r\n");
}

int main()
{
    // stdio_init_all();
    // gpio_init(25);
    // gpio_set_dir(25, GPIO_OUT);
    // while (true) {
    //     printf("Hello, world!\n");
    //     sleep_ms(1000);
    //     gpio_put(25, 1);
    //     sleep_ms(1000);
    //     gpio_put(25, 0);
    // }

    vreg_set_voltage(VREG_VOLTAGE_1_30);
    set_sys_clock_khz(315000, false);
    clock_configure(
        clk_peri,
        0,                                                // No glitchless mux
        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, // System PLL on AUX mux
        315000 * 1000,                               // Input frequency
        315000 * 1000                                // Output (must be same as no divider)
    );
    clock_configure(
        clk_hstx,
        0,                                                // No glitchless mux
        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, // System PLL on AUX mux
        315000 * 1000,                               // Input frequency
        315000 / 2 * 1000                                // Output (must be same as no divider)
    );

    // vreg_disable_voltage_limit();
    // vreg_set_voltage(VREG_VOLTAGE_1_50);

    // set_sys_clock_khz(150000, true);

    // // Get the processor sys_clk frequency in Hz
    // uint32_t freq = clock_get_hz(clk_sys);

    // // clk_peri does not have a divider, so input and output frequencies will be the same
    // clock_configure(clk_peri,
    //     0,
    //     CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
    //     freq,
    //     freq);


    // uint vco_freq, post_div1, post_div2;
    // if (!check_sys_clock_khz(150000 >> 1, &vco_freq, &post_div1, &post_div2))
    //     printf("System clock of %u kHz cannot be exactly achieved", 150000 >> 1);
    // freq = 0;
    // freq = vco_freq / (post_div1 * post_div2);
    // freq = 125 * MHZ;

    // // Set the sys PLL to the requested freq
    // // pll_init(pll_sys, PLL_COMMON_REFDIV, vco_freq, post_div1, post_div2);

    // // CLK HSTX = Requested freq
    // clock_configure(clk_hstx,
    //     0,
    //     CLOCKS_CLK_HSTX_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
    //     freq, freq);


    stdio_init_all();

    measure_freqs();

    Game game;

    game.run();

    // maybe shut down and reboot after?

    while (1) {
    }
    return 0;

}


void measure_freqs(void) {
    uint f_pll_sys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY);
    uint f_pll_usb = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_USB_CLKSRC_PRIMARY);
    uint f_rosc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC);

    uint f_clk_sys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS);
    uint f_clk_peri = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_PERI);
    uint f_clk_usb = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_USB);
    uint f_clk_adc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_ADC);
    uint f_clk_hstx = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_HSTX);
#ifdef CLOCKS_FC0_SRC_VALUE_CLK_RTC
    uint f_clk_rtc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_RTC);
#endif

    printf("pll_sys  = %dkHz\n", f_pll_sys);
    printf("pll_usb  = %dkHz\n", f_pll_usb);
    printf("rosc     = %dkHz\n", f_rosc);
    printf("clk_sys  = %dkHz\n", f_clk_sys);
    printf("clk_peri = %dkHz\n", f_clk_peri);
    printf("clk_usb  = %dkHz\n", f_clk_usb);
    printf("clk_adc  = %dkHz\n", f_clk_adc);
    printf("clk_hstx  = %dkHz\n", f_clk_hstx);
#ifdef CLOCKS_FC0_SRC_VALUE_CLK_RTC
    printf("clk_rtc  = %dkHz\n", f_clk_rtc);
#endif

    // Can't measure clk_ref / xosc as it is the ref
}