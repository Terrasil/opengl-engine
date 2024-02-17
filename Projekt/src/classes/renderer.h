#pragma once

#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include "light.h"

class Renderer
{
private:
    Renderer();
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    std::vector<Light*> lights;
public:
    static Renderer& getInstance();
    std::vector<Light*> getLights();
};

#endif 