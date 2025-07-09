#pragma once
#include "tinyengine_scene.h"
#include "tinyengine_framebuffer.h"
#include "tinyengine_renderer_dvi_c_adapter.h"
#include <vector>

class GameScene : public SceneI {
private:
    TinyEngineFrameBuffer& m_framebuffer;
    TinyEngineRendererI& m_renderer;
    // std::vector<TinyEngineSprite> sprites;
public:
    GameScene(TinyEngineFrameBuffer& _framebuffer, TinyEngineRendererI& _renderer)
        : m_framebuffer(_framebuffer), m_renderer(_renderer) {
    };
    ~GameScene() {};
    void create();
    void render();
    void update(double frameTime);
    void destroy();

};