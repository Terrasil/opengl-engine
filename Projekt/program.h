#pragma once

#ifndef PROGRAM_H
#define PROGRAM_H

#include <glew.h>

class Program
{
private:
	GLuint id;
public:
	void init();
	GLuint getId();
};

#endif 