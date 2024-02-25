#pragma once

#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include "light.h"
#include "skybox.h"
#include "multimesh.h"
#include "Terrain.h"
#include "Camera.h"
#include "Water.h"

class Renderer
{
private:
    Renderer();
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    std::vector<Light*> lights;
    std::vector<SkyBox*> skyboxes;
    MultiMesh* trees;
    MultiMesh* rocks;
    Texture* shadowTexture;
    Terrain* terrain;
    Water* water;
    glm::mat4 lightProj = glm::mat4(1.0);
    glm::mat4 lightView = glm::mat4(1.0);
    Camera* playerCamera = new Camera();
    Camera* minimapCamera = new Camera();
    Camera* lightCamera = new Camera();
    double time = 0.0f;
public:
    static Renderer& getInstance();
    std::vector<Light*> getLights();
    void addLights(Light* light);
    std::vector<SkyBox*> getSkyBoxes();
    void addSkyBox(SkyBox* skybox);
    void setTrees(MultiMesh* newTrees);
    MultiMesh* getTrees();
    void setRocks(MultiMesh* newRocks);
    MultiMesh* getRocks();
    Terrain* getTerrain();
    void setTerrain(Terrain* newTerrain);
    Water* getWater();
    void setWater(Water* newWater);
    void increaseTime(double add);
    void setTime(double newTime);
    void setShadowTexture(Texture* newShadowTexture);
    Texture* getShadowTexture();
    double getTime();
    void setLightProj(glm::mat4 newLightProj);
    glm::mat4 getLightProj();
    void setLightView(glm::mat4 newLightView);
    glm::mat4 getLightView();
    Camera* getPlayerCamera();
    Camera* getMinimapCamera();
    Camera* getLightCamera();
};

#endif 