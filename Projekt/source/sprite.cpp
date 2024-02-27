#include "sprite.h"

#include <freeglut.h>
#include "settings.h"

void Sprite::setTexture(Texture* newTexture) {
	texture = newTexture;
}

void Sprite::draw() {
	GLuint programId = program->getId();
	glUseProgram(programId);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->getId());
	glUniform1i(glGetUniformLocation(programId, "tex"), 0);
	glUniform1i(glGetUniformLocation(programId, "style"), Settings::getInstance().postProcessMode);
	glUniform1f(glGetUniformLocation(programId, "offsetX"), 1.0f / glutGet(GLUT_WINDOW_WIDTH));
	glUniform1f(glGetUniformLocation(programId, "offsetY"), 1.0f / glutGet(GLUT_WINDOW_HEIGHT));
	glUniform1i(glGetUniformLocation(programId, "flipX"), flipX);
	glUniform1i(glGetUniformLocation(programId, "flipY"), flipY);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

void Sprite::updateVertices() {
	vertices = {
		glm::vec3((2.0f * (position.x))				/ glutGet(GLUT_WINDOW_WIDTH) - 1.0f, 1.0f - (2.0f * (position.y + size.y))	/ glutGet(GLUT_WINDOW_HEIGHT), -1.0f),
		glm::vec3((2.0f * (position.x + size.x))	/ glutGet(GLUT_WINDOW_WIDTH) - 1.0f, 1.0f - (2.0f * (position.y + size.y))	/ glutGet(GLUT_WINDOW_HEIGHT), -1.0f),
		glm::vec3((2.0f * (position.x))				/ glutGet(GLUT_WINDOW_WIDTH) - 1.0f, 1.0f - (2.0f * (position.y))			/ glutGet(GLUT_WINDOW_HEIGHT), -1.0f),
		glm::vec3((2.0f * (position.x + size.x))	/ glutGet(GLUT_WINDOW_WIDTH) - 1.0f, 1.0f - (2.0f * (position.y + size.y))	/ glutGet(GLUT_WINDOW_HEIGHT), -1.0f),
		glm::vec3((2.0f * (position.x))				/ glutGet(GLUT_WINDOW_WIDTH) - 1.0f, 1.0f - (2.0f * (position.y))			/ glutGet(GLUT_WINDOW_HEIGHT), -1.0f),
		glm::vec3((2.0f * (position.x + size.x))	/ glutGet(GLUT_WINDOW_WIDTH) - 1.0f, 1.0f - (2.0f * (position.y))			/ glutGet(GLUT_WINDOW_HEIGHT), -1.0f),
	};
	glBindVertexArray(VAO);
	glGenBuffers(1, &verticesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* vertices.size(), &(vertices.at(0)), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

void Sprite::setSize(glm::vec2 newSize) {
	size = newSize;
	updateVertices();
}

void Sprite::setPosition(glm::vec2 newPosition) {
	position = newPosition;
	updateVertices();
}

void Sprite::setProgram(Program* newProgram) {
	program = newProgram;
}

void Sprite::setName(std::string newName) {
	name = newName;
}

std::string Sprite::getName() {
	return name;
}

void Sprite::setFlipX(bool newFlipX) {
	flipX = newFlipX;
}
bool Sprite::getFlipX() {
	return flipX;
}
void Sprite::setFlipY(bool newFlipY) {
	flipY = newFlipY;
}
bool Sprite::getFlipY() {
	return flipX;
}

Sprite::Sprite() {
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &verticesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &(vertices.at(0)), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	glBindVertexArray(VAO);
	glGenBuffers(1, &normalsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), &(normals.at(0)), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	glBindVertexArray(VAO);
	glGenBuffers(1, &uvsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, uvsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * uvs.size(), &(uvs.at(0)), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
}