#pragma once

#ifndef SPRITE_H
#define SPRITE_H

#include <vector>
#include <glm.hpp>
#include "program.h"
#include "texture.h"

class Sprite
{
private:
	std::vector<glm::vec3> vertices {
		glm::vec3(-0.5f, -0.5f, -1.0f),
		glm::vec3( 0.5f, -0.5f, -1.0f),
		glm::vec3(-0.5f,  0.5f, -1.0f),
		glm::vec3 (0.5f, -0.5f, -1.0f),
		glm::vec3(-0.5f,  0.5f, -1.0f),
		glm::vec3( 0.5f,  0.5f, -1.0f)
	};
	std::vector<glm::vec2> uvs{
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f)
	};
	std::vector<glm::vec3> normals {
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)
	};
	glm::vec2 size = glm::vec2(0.0f);
	glm::vec2 position = glm::vec2(0.0f);
	GLuint VAO;
	GLuint verticesVBO;
	GLuint uvsVBO;
	GLuint normalsVBO;
	Texture* texture;
public:
	void updateVertices();
	void setTexture(Texture* newTexture);
	void setSize(glm::vec2 newSize);
	void setPosition(glm::vec2 newPosition);
	void draw(Program* program);
	Sprite();
};

#endif 