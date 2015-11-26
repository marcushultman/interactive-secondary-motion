/*
* Copyright © 2015, Marcus Hultman
*/
#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "ModelViewProjectionShader.h"

class CoordGrid
{
public:
	CoordGrid();
	~CoordGrid();

	void draw(const glm::mat4 &view, const glm::mat4 &proj);
private:
	float m_size;
	unsigned int m_subDivisions;

	GLuint m_VAO;
	GLshort m_numIndices;
	const ModelViewProjectionShader *m_pShader;

	void createGrid();
};

