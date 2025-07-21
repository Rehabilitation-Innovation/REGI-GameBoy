#include "GameScene.h"
#include <malloc.h>
void GameScene::create() {
}

GameScene::~GameScene() {
}

void GameScene::destroy() {
};

uint32_t getTotalHeap(void) {
    extern char __StackLimit, __bss_end__;

    return &__StackLimit - &__bss_end__;
}

uint32_t getFreeHeap(void) {
    struct mallinfo m = mallinfo();

    return getTotalHeap() - m.uordblks;
}

extern char __StackLimit, __bss_end__;

struct mallinfo info;
char string[255];

void GameScene::render() {

    sprintf(string, "Total Heap: %d bytes |  Free: %d \n", getTotalHeap(), getFreeHeap());
    m_framebuffer.draw_string(string, 5, 40, 15);

    sprintf(string, "Total Render Time %dms / %dms\n", m_engine.get_profile_render_loop_time(),
        m_engine.get_profile_main_loop_time());
    m_framebuffer.draw_string(string, 5,  50, 15);

    sprintf(string, "Total Update: %dms / %dms\n",
        m_engine.get_profile_update_loop_time(), m_engine.get_profile_main_loop_time());
    m_framebuffer.draw_string(string, 5, 60, 15);

    // sprintf(string, "Total allocated: %d bytes\n", info.uordblks);
    // m_framebuffer.draw_string(string, 5, 60, 15);
    // sprintf(string, "Total free: %d bytes\n", info.fordblks);
    // m_framebuffer.draw_string(string, 5, 70, 15);
    // sprintf(string, "Total heap size: %d bytes\n", info.arena);
    // m_framebuffer.draw_string(string, 5, 80, 15);
    // sprintf(string, "Largest free block: %d bytes\n", info.ordblks);
    // m_framebuffer.draw_string(string, 5, 90, 15);
    sprintf(string, "FPS: %d\r\n", m_engine.get_fps());
    m_framebuffer.draw_string(string, 5, 10, 15);
    sprintf(string, "Press W, A, S, D for movement.\r\n");
    m_framebuffer.draw_string(string, 5, 20, 15);
    sprintf(string, "Press X to quit the game.\r\n");
    m_framebuffer.draw_string(string, 5, 30, 15);

};

void GameScene::update(double frameTime) {
    info = mallinfo();
};
