#include "assets_repository.h"

#include <iostream>
#include <glew.h>
#include <fstream>

//#include "utilities.hpp"

// MESH

Mesh* AssetsRepository::findMesh(std::string name) {
    std::map<std::string, Mesh*>::iterator searchedMesh;
    searchedMesh = meshes.find(name);
    if (searchedMesh != meshes.end()) {
        return searchedMesh->second;
    }
    return nullptr;
}

void AssetsRepository::registerMesh(std::string name, std::string path) {
    if (findMesh(name) != nullptr) {
        std::cout << "AssetsRepository::registerMesh - Mesh '" << name << "' already exist." << std::endl;
    }
    Mesh* newMesh = new Mesh();
    newMesh->load(path);
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

// PROGRAM

Program* AssetsRepository::findProgram(std::string name) {
    std::map<std::string, Program*>::iterator searchedProgram;
    searchedProgram = programs.find(name);
    if (searchedProgram != programs.end()) {
        return searchedProgram->second;
    }
    return nullptr;
}

void AssetsRepository::registerProgram(std::string name) {
    if (findProgram(name) != nullptr) {
        std::cout << "AssetsRepository::registerProgram - Program '" << name << "' already exist." << std::endl;
    }
    Program* newProgram = new Program();
    newProgram->init();
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

// TEXTURE

Texture* AssetsRepository::findTexture(std::string name) {
    std::map<std::string, Texture*>::iterator searchedTexture;
    searchedTexture = textures.find(name);
    if (searchedTexture != textures.end()) {
        return searchedTexture->second;
    }
    return nullptr;
}

void AssetsRepository::registerTexture(std::string name, std::string path, GLuint format) {
    if (findTexture(name) != nullptr) {
        std::cout << "AssetsRepository::registerTexture - Texture '" << name << "' already exist." << std::endl;
    }
    Texture* newTexture = new Texture();
    newTexture->load(path, format);
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