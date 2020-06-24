/*
* Copyright (c) 2015, Marcus Hultman
*/
#pragma once

#include "Constraint.h"

#include "TwVars.h"
extern TwVars g_twVar;

class CollisionConstraint : public Constraint
{
public:
	CollisionConstraint(unsigned int i, glm::vec3 target)
	{
		m_indices = { i };
		m_target = target;
	}

	void projectConstraint(unsigned int solvIt, std::vector<glm::vec3> &p,
		std::vector<glm::vec3> &s) const
	{
		unsigned int i = m_indices[0];
		p[i] = m_target;
	}
	
private:
	glm::vec3 m_target;
};