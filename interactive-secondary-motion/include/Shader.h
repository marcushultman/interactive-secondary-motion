/*
* Copyright © 2015, Marcus Hultman
*/
#pragma once

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>

class Shader
{
public:
	Shader(const GLchar* VSPath, const GLchar* FSPath);
	~Shader();

	const GLuint getProgram() const;

protected:
	GLuint m_program;
};