#pragma once

#ifndef SKYBOX_H
#define SKYBOX_H

#include <map>
#include <string>

#include <glew.h>
#include <glm.hpp>

#include "skybox.h"
#include "program.h"
#include "texture.h"

class SkyBox {
private:
	std::map<GLenum, std::string> texturePaths;
	GLuint texture;
	Program* program; 
	GLuint VAO;
	glm::vec3 position = glm::vec3(0.0);
	float size = 100.0;
	GLfloat positions[24] = {
		 1.0f,  1.0f,  1.0f,	// 0
		-1.0f,  1.0f,  1.0f,	// 1
		-1.0f, -1.0f,  1.0f,	// 2
		 1.0f, -1.0f,  1.0f,	// 3
		 1.0f,  1.0f, -1.0f,	// 4
		-1.0f,  1.0f, -1.0f,	// 5
		-1.0f, -1.0f, -1.0f,	// 6
		 1.0f, -1.0f, -1.0f		// 7
	};
	GLuint indices[36] = {
		5, 0, 1,
		5, 4, 0,
		2, 0, 3,
		2, 1, 0,
		7, 0, 4,
		7, 3, 0,
		3, 6, 2,
		3, 7, 6,
		1, 2, 6,
		1, 6, 5,
		4, 5, 6,
		4, 6, 7
	};
public:
	void setTopTexture(std::string topTexturePath);
	void setBottomTexture(std::string bottomTexturePath);
	void setFrontTexture(std::string frontTexturePath);
	void setBackTexture(std::string backTexturePath);
	void setLeftTexture(std::string leftTexturePath);
	void setRightTexture(std::string rightTexturePath);
	GLuint getTextureId();
	void setProgram(Program* newProgram);
	void init();
	void draw(glm::mat4 matProj, glm::mat4 matView);
	void setPosition(glm::vec3 newPosition);
	glm::vec3 getPosition();
	void setSize(float newSize);
	float getSize();
};

#endif 