#pragma once
#include "tinyengine_scene.h"
#include "tinyengine_sprite.h"
#include "tinyengine_framebuffer.h"
#include "tinyengine_renderer_dvi.h"
#include <vector>

class GameScene : public SceneI {
protected: // debatable
    TinyEngineFrameBuffer &m_framebuffer;
    TinyEngineRendererI &m_renderer;
    TinyEngine &m_engine;
    std::vector<Sprite *> sprites;

public:
    GameScene(TinyEngineFrameBuffer &_framebuffer, TinyEngineRendererI &_renderer, TinyEngine &_engine)
        : m_framebuffer(_framebuffer), m_renderer(_renderer), m_engine(_engine) {
    };

    ~GameScene();

    void create() override;

    void render() override;

    void update(double frameTime) override;

    void destroy() override;
};
