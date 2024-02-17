#pragma once

#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <glew.h>

class Texture
{
private:
	GLuint id;
	unsigned char* data;
	int width;
	int height;
	int comp;
	GLuint format;
public:
	void load(std::string path, GLuint loadFormat);
	void setId(GLuint newTextureId);
	GLuint getId();
	int getWidth();
	int getHeight();
	int getFormat();
};

#endif 