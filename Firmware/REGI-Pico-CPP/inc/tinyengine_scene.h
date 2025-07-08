#pragma once

class SceneI {
public:
    virtual ~SceneI() = default;
    virtual void create() = 0;
    virtual void render() = 0;
    virtual void update(double frameTime) = 0;
    virtual void destroy() = 0;
};