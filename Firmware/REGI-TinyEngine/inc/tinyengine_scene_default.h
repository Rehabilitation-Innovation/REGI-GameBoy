#pragma once
#include "tinyengine_scene.h"
class TinyEngineSceneDefault : public SceneI {
public:
    TinyEngineSceneDefault() { create(); };
    ~TinyEngineSceneDefault() { destroy(); };

    void render();
    void update(double frameTime);
    void create();
    void destroy();
};