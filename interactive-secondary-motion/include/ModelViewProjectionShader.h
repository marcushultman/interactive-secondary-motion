/*
* Copyright (c) 2015, Marcus Hultman
*/
#pragma once

#include "Shader.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class ModelViewProjectionShader : public Shader
{
public:
	ModelViewProjectionShader(const GLchar* VSPath, const GLchar* FSPath);
	~ModelViewProjectionShader();

	void setModelMatrix(const glm::mat4&) const;
	void setViewMatrix(const glm::mat4&) const;
	void setProjectionMatrix(const glm::mat4&) const;
};

