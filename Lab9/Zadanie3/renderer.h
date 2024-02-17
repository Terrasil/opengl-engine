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
    Texture* shadowTexture;
    glm::mat4 lightProj = glm::mat4(1.0);
    glm::mat4 lightView = glm::mat4(1.0);
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
    void setShadowTexture(Texture* newShadowTexture);
    Texture* getShadowTexture();
    double getTime();
    void setLightProj(glm::mat4 newLightProj);
    glm::mat4 getLightProj();
    void setLightView(glm::mat4 newLightView);
    glm::mat4 getLightView();
};

#endif 