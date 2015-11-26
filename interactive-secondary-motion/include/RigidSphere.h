/*
* Copyright © 2015, Marcus Hultman
*/
#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include "ModelViewProjectionShader.h"

class RigidSphere
{
public:
	RigidSphere();
	~RigidSphere();

	void reset();
	void reset(bool startScenario);

	void update(double elapsedTime);
	void draw(const glm::mat4 &view, const glm::mat4 &proj);
	void draw(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &proj);

	glm::vec3 getPosition();
	float getRadius();
	float getRadius2();

	bool startCollision();

private:
	glm::vec3 m_position;
	glm::vec3 m_prevPosition;

	bool m_hasCollided;
	float m_radius, m_radius2;

	// TODO: Outsource rendering
	GLuint m_VAO;
	GLshort m_numIndices;
	ModelViewProjectionShader *m_pShader;

	void createMesh(float radius, unsigned int rings, unsigned int sectors);
};

