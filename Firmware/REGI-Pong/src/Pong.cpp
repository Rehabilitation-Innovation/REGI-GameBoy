//
// Created by GRHRehabTech on 2025-08-07.
//

#include "Pong.h"
#include "tinyengine.h"
#include "cute_c2.h"
#include "hardware/watchdog.h"
#include "pico/multicore.h"
#include "distance_sensor.h"

int move_up, move_down, move_left, move_right;

struct paddle {
    float x = 0;
    float y = 0;

    float move_speed = 200; // pixels
};

struct ball {
    float x = 0;
    float y = 0;
    float dx = 0, dy = 0;
    float move_speed = 100;
};

PongScene::~PongScene() {
}

paddle a, b;
ball ba;

int timeout = 26100;


uint64_t getPulse(uint trigPin, uint echoPin)
{
    gpio_put(trigPin, 1);
    sleep_us(10);
    gpio_put(trigPin, 0);

    uint64_t width = 0;

    while (gpio_get(echoPin) == 0) tight_loop_contents();
    absolute_time_t startTime = get_absolute_time();
    while (gpio_get(echoPin) == 1)
    {
        // printf("%d\r\n", width);
        width++;
        sleep_us(1);
        if (width > timeout) return 0;
    }
    absolute_time_t endTime = get_absolute_time();

    return absolute_time_diff_us(startTime, endTime);
}

uint64_t getCm(uint trigPin, uint echoPin)
{
    uint64_t pulseLength = getPulse(trigPin, echoPin);
    return pulseLength / 29 / 2;
}

uint64_t getInch(uint trigPin, uint echoPin)
{
    uint64_t pulseLength = getPulse(trigPin, echoPin);
    return (long)pulseLength / 74.f / 2.f;
}

uint32_t dist = 0;

DistanceSensor d(pio0, 0, 40);
void core1_entry() {

    // multicore_fifo_push_blocking(FLAG_VALUE);

    // uint32_t g = multicore_fifo_pop_blocking();

    // if (g != FLAG_VALUE)
    //     printf("Hmm, that's not right on core 1!\n");
    // else
    //     printf("Its all gone well on core 1!");
    // save_and_disable_interrupts();
    while (1) {
        // printf("%d\r\n", dist);
        gpio_put(0, 1);
        // dist = getCm(40, 41);
        gpio_put(0, 0);
        sleep_ms(500);
    }
    // tight_loop_contents();
}

void PongScene::create() {
    GameScene::create();
    gpio_init(40);
    gpio_init(41);
    gpio_set_dir(40, GPIO_OUT);
    gpio_set_dir(41, GPIO_IN);
    gpio_init(0);
    gpio_set_dir(0, GPIO_OUT);
    // d = DistanceSensor(pio0, 0, 2);
    // d.TriggerRead();
    // telog("Reading %d", d.distance);
    // setupUltrasonicPins();
    // telog("Reading %d", getCm());

    m_engine.bind_serial_input_event('w', [&] {
        move_down = 1;
        });

    m_engine.bind_serial_input_event('s', [&] {
        move_up = 1;
        });

    m_engine.bind_serial_input_event('r', [&] {
        watchdog_reboot(0, 0, 10);
        // move_up = 1;
        });
    multicore_launch_core1(core1_entry);
    ba.dx = 1;
    ba.dy = 1;
    ba.x = 320 / 2;
    ba.y = 240 / 2;

    b.x = 320 - 10;
}

void PongScene::render() {
    GameScene::render();
    m_framebuffer.clear(0);

    m_framebuffer.draw_filled_rectangle(a.x, a.y, 10, 50, 15);

    m_framebuffer.draw_filled_rectangle(b.x, b.y, 10, 50, 15);

    m_framebuffer.draw_filled_rectangle(ba.x, ba.y, 10, 10, 31);

    // m_renderer.wait_for_vsync();
    m_framebuffer.swap_blocking();
}

c2AABB ballbox, pada, padb;



void PongScene::update(double frameTime) {
    // static DistanceSensor d{ pio0, 1, 2 };
    GameScene::update(frameTime);
    // d.TriggerRead();
    // while (d.is_sensing) {
    //     sleep_us(10);
// }


    // telog("Reading %d", d.distance);
    // // Trigger background sense
    // // wait for sensor to get a result
    if (!d.is_sensing) {
        telog("Reading %d", d.distance);
        a.y = d.distance;
        d.TriggerRead();
    }
    // telog("Reading %d", d.distance);

    // a.y = dist;

    ballbox.max.x = ba.x + 10;
    ballbox.max.y = ba.y + 10;

    ballbox.min.x = ba.x;
    ballbox.min.y = ba.y;

    pada.max.x = a.x + 10;
    pada.max.y = a.y + 50;
    pada.min.x = a.x;
    pada.min.y = a.y;

    padb.max.x = b.x + 10;
    padb.max.y = b.y + 50;
    padb.min.x = b.x;
    padb.min.y = b.y;

    if (c2AABBtoAABB(pada, ballbox) || c2AABBtoAABB(padb, ballbox)) {
        ba.dx *= -1;
    }

    ba.x += ba.dx * ba.move_speed * frameTime;

    if (ba.x >= 320 - 10) {
        // ba.dx *= -1;
        ba.x = 320 / 2;
        ba.y = 240 / 2;
    }
    if (ba.x <= 0) {
        // ba.dx *= -1;
        ba.x = 320 / 2;
        ba.y = 240 / 2;
    }

    ba.y += ba.dy * ba.move_speed * frameTime;
    if (ba.y >= 240 - 10) {
        ba.dy *= -1;
        // ba.x = 320 / 2;
        // ba.y = 240 / 2;
    }

    if (ba.y <= 0) {
        ba.dy *= -1;
        // ba.x = 320 / 2;
        // ba.y = 240 / 2;
    }

    if (move_up) {
        a.y += a.move_speed * frameTime;
        move_up = 0;
    }
    if (move_down) {
        a.y -= a.move_speed * frameTime;
        move_down = 0;
    }

    b.y = a.y;

    if (a.x > 320 - 10) a.x = 320 - 10;

    if (a.y > 240 - 50) a.y = 240 - 50;

    if (a.x < 0) a.x = 0;

    if (a.y < 0) a.y = 0;

    if (b.x > 320 - 10) b.x = 320 - 10;

    if (b.y > 240 - 50) b.y = 240 - 50;

    if (b.x < 0) b.x = 0;

    if (b.y < 0) b.y = 0;

    if (ba.x > 320 - 10) ba.x = 320 - 10;

    if (ba.y > 240 - 10) ba.y = 240 - 10;

    if (ba.x <= 0) ba.x = 0;

    if (ba.y <= 0) ba.y = 0;
}

void PongScene::destroy() {
    GameScene::destroy();
}
