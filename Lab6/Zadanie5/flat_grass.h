#pragma once
#include <glew.h>

GLfloat flat_grass_vertices_coord[] =
{
	-1.0f, -1.0f,  0.0f,
	 1.0f, -1.0f,  0.0f,
	-1.0f,  1.0f,  0.0f,
	 1.0f, -1.0f,  0.0f,
	-1.0f,  1.0f,  0.0f,
	 1.0f,  1.0f,  0.0f,
	 0.0f, -1.0f,  1.0f,
	 0.0f, -1.0f, -1.0f,
	 0.0f,  1.0f,  1.0f,
	 0.0f, -1.0f, -1.0f,
	 0.0f,  1.0f,  1.0f,
	 0.0f,  1.0f, -1.0f,
};


GLfloat flat_grass_vertices_normal[] =
{
	0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
};


GLfloat flat_grass_vertices_uv[] =
{
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
};
