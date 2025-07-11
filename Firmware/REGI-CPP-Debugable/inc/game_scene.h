#pragma once
#include "tinyengine_scene.h"
#include "tinyengine_sprite.h"
#include "tinyengine_framebuffer.h"
#include "tinyengine_renderer_dvi.h"
#include <vector>

class GameScene : public SceneI {
    TinyEngineFrameBuffer& m_framebuffer;
    TinyEngineRendererI& m_renderer;

    std::vector<Sprite*> sprites;
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