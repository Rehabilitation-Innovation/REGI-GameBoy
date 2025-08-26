//
// Created by GRHRehabTech on 2025-08-07.
//

#include "BrickBreaker.h"
#include "tinyengine.h"
#include "cute_c2.h"
#include "hardware/watchdog.h"
#include "pico/multicore.h"
#include "distance_sensor.h"
#include "TinyEngineUI.h"

int move_up = 0, move_down = 0, move_left = 0, move_right = 0, toggle_move_up = 0, toggle_move_up2 = 0;

struct paddle {
    float x = 0;
    float y = 0;

    float move_speed = 100; // pixels
};

struct ball {
    float x = 0;
    float y = 0;
    float dx = 0, dy = 0;
    float move_speed = 100;
};

BrickBreakerScene::~BrickBreakerScene() {
}

paddle a, b;
ball ba;

/**
 * ------------------------------------------
 * High Score:               Pongs:
 * Time:                Missed Pongs:
 * ------------------------------------------
 */
TinyEngineUIText score(5, 5, "Score: ", 15);
TinyEngineUIText time_(5, 30, "Time: ", 15);
TinyEngineUIText pongs(90, 5, "Pongs: ", 15);
TinyEngineUIText missed_pongs(90, 30, "Missed Pongs: ", 15);

TinyEngineUITextBox scoreboard(70, 180, 210, 50, 45);

TinyEngineUIText input(0, 20, "Input: ", 15);

void BrickBreakerScene::create() {
    GameScene::create();
    gpio_init(40);
    gpio_init(41);
    gpio_set_dir(40, GPIO_OUT);
    gpio_set_dir(41, GPIO_IN);
    gpio_init(0);
    gpio_set_dir(0, GPIO_OUT);

    scoreboard.add_text("s", score);
    scoreboard.add_text("j", pongs);
    scoreboard.add_text("t", time_);
    scoreboard.add_text("mj", missed_pongs);

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

    m_engine.bind_gpio_input_event(2, [&] {
        toggle_move_up = !toggle_move_up;
        // printf("a\r\n");
        });

    m_engine.bind_gpio_input_event(24, [&] {
        toggle_move_up2 = !toggle_move_up2;
        // printf("a\r\n");
        });

    m_engine.bind_serial_input_event('r', [&] {
        watchdog_reboot(0, 0, 10);
        // move_up = 1;
        });
    ba.dx = 1;
    ba.dy = 1;
    ba.x = 320 / 2;
    ba.y = 240 / 2;

    b.x = 320 - 10;
}

void BrickBreakerScene::render() {
    GameScene::render();
    m_framebuffer.clear(0);

    scoreboard.render(m_framebuffer);

    m_framebuffer.draw_filled_rectangle(a.x, a.y, 10, 50, 15);

    m_framebuffer.draw_filled_rectangle(b.x, b.y, 10, 50, 15);

    m_framebuffer.draw_filled_rectangle(ba.x, ba.y, 10, 10, 31);

    m_renderer.wait_for_vsync();
    m_framebuffer.swap_blocking();
}

c2AABB ballbox, pada, padb;
uint16_t pong_counter = 0, miss = 0, hscore = 0;
char temp[255] = { 0 };
double t = 0;

void BrickBreakerScene::update(double frameTime) {
    // static DistanceSensor d{ pio0, 1, 2 };
    GameScene::update(frameTime);
    // d.TriggerRead();
    // while (d.is_sensing) {
    //     sleep_us(10);
// }
    t += frameTime;
    sprintf(temp, "Score:%d", hscore);
    score.set_text(temp);
    sprintf(temp, "Pongs:%d", pong_counter);
    pongs.set_text(temp);
    sprintf(temp, "Time:%1.0fs", t);
    time_.set_text(temp);
    sprintf(temp, "Missed Pongs:%d", miss);
    missed_pongs.set_text(temp);

    // telog("Reading %d", d.distance);
    // // Trigger background sense
    // // wait for sensor to get a result
    // if (!d.is_sensing) {
    //     telog("Reading %d", d.distance);
    //     a.y = d.distance;
    //     d.TriggerRead();
    // }
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
        pong_counter += 1;
    }

    ba.x += ba.dx * ba.move_speed * frameTime;

    if (ba.x >= 320 - 10) {
        // ba.dx *= -1;
        ba.x = 320 / 2;
        ba.y = 240 / 2;
        hscore += 1;
    }
    if (ba.x <= 0) {
        // ba.dx *= -1;
        ba.x = 320 / 2;
        ba.y = 240 / 2;
        hscore = 0;
        miss += 1;
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

    if (toggle_move_up) {
        move_up = 1;
        move_down = 0;
    }
    else {
        move_up = 0;
        move_down = 1;
    }


    if (move_down) {
        a.y -= a.move_speed * frameTime;
        move_down = 0;
    }

    if (toggle_move_up2) {
        b.y += b.move_speed * frameTime;
    }
    else {
        b.y -= b.move_speed * frameTime;
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

void BrickBreakerScene::destroy() {
    GameScene::destroy();
}
