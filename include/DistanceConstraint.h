/*
* Copyright (c) 2015, Marcus Hultman
*/
#pragma once

#include "Constraint.h"
#include <algorithm>

#include "TwVars.h"
extern TwVars g_twVar;

class DistanceConstraint : public Constraint {
public:
	DistanceConstraint(unsigned int i1, unsigned int i2, std::vector<float> &w)
	{
		m_indices = { i1, i2 };
		std::sort(m_indices.begin(), m_indices.end());
		m_stiffness = (w[i1] + w[i2]) / 2.0f;

		m_stiffness = 1.000f;
	}
	void projectConstraint(unsigned int solvIt, std::vector<glm::vec3> &p, std::vector<glm::vec3> &s) const {
		unsigned int i1 = m_indices[0];
		unsigned int i2 = m_indices[1];

		glm::vec3 diff = p[i2] - p[i1];
		float diffDist = glm::length(diff);
		glm::vec3 diffNorm = diff / diffDist;

		glm::vec3 dp = diffNorm * (diffDist - glm::length(s[i2] - s[i1])) / 2.0f;

		float k = 1 - pow(1 - g_twVar.constraint_stiffness, 1 / (float) solvIt);
		dp *= k;
		
		p[i1] += dp;
		p[i2] -= dp;
	}
};