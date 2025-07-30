//
// Created by abina on 7/18/2025.
//

#pragma once
#include "tinyengine_framebuffer.h"

class TinyEngineRenderableI
{
public:
    TinyEngineRenderableI() {};
    virtual ~TinyEngineRenderableI() = default;

    virtual void render(TinyEngineFrameBuffer _framebuffer) = 0;
    virtual void update(double frameTime) = 0;
};
