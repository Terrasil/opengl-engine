#pragma once

#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include "light.h"
#include "skybox.h"

class Renderer
{
private:
    Renderer();
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    std::vector<Light*> lights;
    std::vector<SkyBox*> skyboxes;
public:
    static Renderer& getInstance();
    std::vector<Light*> getLights();
    void addLights(Light* light);
    std::vector<SkyBox*> getSkyBoxes();
    void addSkyBox(SkyBox* skybox);
};

#endif 