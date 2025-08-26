#include "tinyengine.h"
#include "pico/stdlib.h"
#include <pico/time.h>
#include "hardware/gpio.h"
#include "hardware/exception.h" 

#define UART_ID uart0

volatile static int chars_rxed = 0;
std::vector<uint8_t>* serial_input_queue;

// // callback when data is received on a CDC interface
// void tud_cdc_rx_cb(uint8_t itf)
// {
//     // allocate buffer for the data in the stack
//     uint8_t buf[CFG_TUD_CDC_RX_BUFSIZE];

//     // printf("RX CDC %d\n", itf);

//     // read the available data
//     // | IMPORTANT: also do this for CDC0 because otherwise
//     // | you won't be able to print anymore to CDC0
//     // | next time this function is called
//     uint32_t count = tud_cdc_n_read(itf, buf, sizeof(buf));

//     // check if the data was received on the second cdc interface
//     if (itf == 1) {
//         // process the received data
//         printf("%c", buf[0]);
//         serial_input_queue->emplace_back(buf[0]);
//         chars_rxed++;
//         // buf[count] = 0; // null-terminate the string
//         // now echo data back to the console on CDC 0
//         // printf("Received on CDC 1: %s\n", buf);
//         //
//         // // and echo back OK on CDC 1
//         // tud_cdc_n_write(itf, (uint8_t const *) "OK\r\n", 4);
//         // tud_cdc_n_write_flush(itf);
//     }
// }
static void hardfault() {
    telog("HARD FAULT!!");
    gpio_init(0);
    gpio_set_dir(0, GPIO_OUT);
    gpio_init(1);
    gpio_set_dir(1, GPIO_OUT);
    gpio_init(6);
    gpio_set_dir(6, GPIO_OUT);
    gpio_init(7);
    gpio_set_dir(7, GPIO_OUT);
    gpio_init(9);
    gpio_set_dir(9, GPIO_OUT);
    gpio_init(45);
    gpio_set_dir(45, GPIO_OUT);

    while (1) {
        gpio_put(0, 1);
        sleep_ms(100);

        gpio_put(1, 1);
        sleep_ms(100);

        gpio_put(6, 1);
        sleep_ms(100);

        gpio_put(7, 1);
        sleep_ms(100);

        gpio_put(9, 1);
        sleep_ms(100);

        gpio_put(45, 1);
        sleep_ms(100);

        gpio_put(0, 0);
        sleep_ms(100);

        gpio_put(1, 0);
        sleep_ms(100);

        gpio_put(6, 0);
        sleep_ms(100);

        gpio_put(7, 0);
        sleep_ms(100);

        gpio_put(9, 0);
        sleep_ms(100);

        gpio_put(45, 0);
        sleep_ms(100);
    }

}


void set_frequency() {
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
    exception_set_exclusive_handler(HARDFAULT_EXCEPTION, hardfault);
}

// RX interrupt handler
void on_uart_rx()
{
    while (uart_is_readable(UART_ID))
    {
        uint8_t ch = uart_getc(UART_ID);
        serial_input_queue->emplace_back(ch);
        chars_rxed++;
    }
}

std::vector<uint8_t>* gpio_input_queue;
std::unordered_map<uint8_t, uint32_t> gpio_debouce_map;
static uint32_t gpio_debounce = 0;

void gpio_callback(uint gpio, uint32_t events) {

    // if (!gpio_debounce) {
    //     gpio_debounce = time_us_32();
    //     return;
    // }
    // if ((uint32_t)250000 <= (time_us_32() - gpio_debounce)) {
    //     gpio_input_queue->emplace_back(gpio);
    //     gpio_debounce = 0;
    // }

    if (!gpio_debouce_map[gpio]) {
        gpio_debouce_map[gpio] = time_us_32();
        return;
    }
    if ((uint32_t)250000 <= (time_us_32() - gpio_debouce_map[gpio])) {
        gpio_input_queue->emplace_back(gpio);
        gpio_debouce_map[gpio] = 0;
    }

}

tinyengine_status_t TinyEngine::init()
{

    serial_input_queue = &m_serial_input_buffer;
    // Set up a RX interrupt
    // We need to set up the handler first
    // Select correct interrupt for the UART we are using
    int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;
    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(UART_ID, true, false);

    gpio_input_queue = &m_gpio_input_buffer;

    pre_init_clbk();

    return TINYENGINE_OK;
}

tinyengine_status_t TinyEngine::start()
{
    return TINYENGINE_OK;
}

void TinyEngine::render(double frametime)
{
}

void TinyEngine::update(double frametime)
{
    // engine_handle->update_clbk(frametime);
}

void TinyEngine::update_inputs()
{
    if (!m_gpio_input_buffer.empty())
    {
        for (uint8_t gpio : m_gpio_input_buffer)
        {
            if (m_gpio_input_events.contains(gpio))
            {
                m_gpio_input_events[gpio]();
            }
        }
        m_gpio_input_buffer.clear();
    }

    if (!m_serial_input_buffer.empty())
    {
        for (uint8_t chr : m_serial_input_buffer)
        {
            if (m_serial_input_events.contains(chr))
            {
                m_serial_input_events[chr]();
            }
        }
        m_serial_input_buffer.clear();
    }
}

void TinyEngine::bind_gpio_input_event(uint8_t _gpio, std::function<void()> _event_callback)
{
    gpio_init(_gpio);
    gpio_set_dir(_gpio, GPIO_IN);
    gpio_pull_up(_gpio);

    gpio_set_irq_enabled_with_callback(_gpio,
        //GPIO_IRQ_EDGE_RISE |
        GPIO_IRQ_EDGE_FALL,
        true, &gpio_callback);

    m_gpio_input_events.emplace(_gpio, _event_callback);
    gpio_debouce_map.emplace(_gpio, 0);
}

void TinyEngine::bind_serial_input_event(uint8_t _char, std::function<void()> _event_callback)
{
    m_serial_input_events.emplace(_char, _event_callback);
}

tinyengine_status_t TinyEngine::start_loop()
{

    double lastTime = ((double)to_ms_since_boot(get_absolute_time())) / ((double)1000);
    double unproccessedTime = 0;
    double passedTime = 0;
    double startTime = 0;
    int render = 1;
    int frames = 0;
    int framerate = 60;
    double frametime = (double)1 / (double)framerate;
    uint32_t startus = to_us_since_boot(get_absolute_time());
    double frameCounter = 0;

    uint32_t profile_main_loop_start = 0, profile_main_loop_end = 0;

    uint32_t profile_render_loop_start = 0, profile_render_loop_end = 0;

    uint32_t profile_update_loop_start = 0, profile_update_loop_end = 0;

    while (1)
    {

        profile_main_loop_time = profile_main_loop_end - profile_main_loop_start;

        profile_main_loop_start = to_us_since_boot(get_absolute_time()) / 1000;
        //isRunning
        startTime = ((double)to_ms_since_boot(get_absolute_time())) / ((double)1000);
        passedTime = startTime - lastTime;
        lastTime = startTime;
        unproccessedTime += passedTime;
        frameCounter += passedTime;
        render = 1; // run uncapped
        while ((unproccessedTime > frametime))
        {
            render = 1;
            // telog("Running");
            unproccessedTime -= frametime;
            // engine_handle->update_clbk(frametime);
            // tinyengine_update(engine_handle, frametime);
            profile_update_loop_time = profile_update_loop_end - profile_update_loop_start;

            profile_update_loop_start = to_us_since_boot(get_absolute_time()) / 1000;
            update_clbk(frametime);
            update_inputs();
            profile_update_loop_end = to_us_since_boot(get_absolute_time()) / 1000;

            if (frameCounter >= 1)
            {
                fps = frames;
                telog("FPS: %d", frames);
                printf("Test\r\n");
                frameCounter = 0;
                frames = 0;
            }
        }
        if (render)
        {
            profile_render_loop_time = profile_render_loop_end - profile_render_loop_start;

            profile_render_loop_start = to_us_since_boot(get_absolute_time()) / 1000;
            // engine_handle->render_clbk(frametime);
            // tinyengine_render(engine_handle, frametime);
            render_clbk();
            frames += 1;

            profile_render_loop_end = to_us_since_boot(get_absolute_time()) / 1000;

        }
        else
        {
            // sleep here for rtos;
            // sleep_ms(1);
        }

        profile_main_loop_end = to_us_since_boot(get_absolute_time()) / 1000;

    }
    return TINYENGINE_OK;
}
