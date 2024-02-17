#pragma once

#ifndef ASSETSREPOSITORY_H
#define ASSETSREPOSITORY_H

#include <map>
#include <string>
#include "mesh.h"
#include "program.h"

class AssetsRepository
{
private:
	std::map<std::string, Mesh*> meshes;
	Mesh* findMesh(std::string name);
	std::map<std::string, Program*> programs;
	Program* findProgram(std::string name);
	std::map<std::string, Texture*> textures;
	Texture* findTexture(std::string name);
public:
	void registerMesh(std::string name, std::string path);
	Mesh* getMesh(std::string name);
	void registerProgram(std::string name);
	Program* getProgram(std::string name);
	void registerTexture(std::string name, std::string path, GLuint format);
	Texture* getTexture(std::string name);
};

#endif 