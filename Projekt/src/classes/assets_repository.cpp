#include "assets_repository.h"

#include <iostream>
#include <glew.h>

Mesh* AssetsRepository::findMesh(std::string name) {
    std::map<std::string, Mesh*>::iterator searchedMesh;
    searchedMesh = meshes.find(name);
    if (searchedMesh != meshes.end()) {
        return searchedMesh->second;
    }
    return nullptr;
}

void AssetsRepository::registerMesh(std::string name, std::string path) {
    Mesh* newMesh = new Mesh();
    newMesh->load(path);
    if (findMesh(name) != nullptr) {
        std::cout << "AssetsRepository::registerMesh - Mesh '" << name << "' already exist." << std::endl;
    }
    meshes[name] = newMesh;
}

Mesh* AssetsRepository::getMesh(std::string name) {
    Mesh* findedMesh = findMesh(name);
    if (findedMesh != nullptr) {
        return findedMesh;
    }
    std::cout << "AssetsRepository::getMesh - Mesh '" << name << "' not found." << std::endl;
    return nullptr;
}

Program* AssetsRepository::findProgram(std::string name) {
    std::map<std::string, Program*>::iterator searchedProgram;
    searchedProgram = programs.find(name);
    if (searchedProgram != programs.end()) {
        return searchedProgram->second;
    }
    return nullptr;
}

void AssetsRepository::registerProgram(std::string name) {
    Program* newProgram = new Program();
    newProgram->init();
    if (findProgram(name) != nullptr) {
        std::cout << "AssetsRepository::registerProgram - Program '" << name << "' already exist." << std::endl;
    }
    programs[name] = newProgram;
}

Program* AssetsRepository::getProgram(std::string name) {
    Program* findedProgram = findProgram(name);
    if (findedProgram != nullptr) {
        return findedProgram;
    }
    std::cout << "AssetsRepository::getProgram - Program '" << name << "' not found." << std::endl;
    return nullptr;
}

Texture* AssetsRepository::findTexture(std::string name) {
    std::map<std::string, Texture*>::iterator searchedTexture;
    searchedTexture = textures.find(name);
    if (searchedTexture != textures.end()) {
        return searchedTexture->second;
    }
    return nullptr;
}

void AssetsRepository::registerTexture(std::string name, std::string path, GLuint format) {
    Texture* newTexture = new Texture();
    newTexture->load(path, format);
    if (findTexture(name) != nullptr) {
        std::cout << "AssetsRepository::registerTexture - Texture '" << name << "' already exist." << std::endl;
    }
    textures[name] = newTexture;
}

Texture* AssetsRepository::getTexture(std::string name) {
    Texture* findedTexture = findTexture(name);
    if (findedTexture != nullptr) {
        return findedTexture;
    }
    std::cout << "AssetsRepository::getTexture - Texture '" << name << "' not found." << std::endl;
    return nullptr;
}