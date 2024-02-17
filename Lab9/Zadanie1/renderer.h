#pragma once

#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include "light.h"
#include "skybox.h"
#include "multimesh.h"

class Renderer
{
private:
    Renderer();
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    std::vector<Light*> lights;
    std::vector<SkyBox*> skyboxes;
    MultiMesh* multiMesh;
    double time = 0.0f;
public:
    static Renderer& getInstance();
    std::vector<Light*> getLights();
    void addLights(Light* light);
    std::vector<SkyBox*> getSkyBoxes();
    void addSkyBox(SkyBox* skybox);
    void setMultiMesh(MultiMesh* newMultiMesh);
    MultiMesh* getMultiMesh();
    void increaseTime(double add);
    void setTime(double newTime);
    double getTime();
};

#endif 