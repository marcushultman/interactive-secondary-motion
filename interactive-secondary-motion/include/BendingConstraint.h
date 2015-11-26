/*
* Copyright © 2015, Marcus Hultman
*/
#pragma once

#include "Constraint.h"
#include <algorithm>

#include "TwVars.h"
extern TwVars g_twVar;

class BendingConstraint : public Constraint {
public:
	BendingConstraint(unsigned int i1, unsigned int i2,
		unsigned int i3, unsigned int i4, std::vector<float> &w)
	{
		m_indices = { i1, i2, i3, i4 };
		std::sort(m_indices.begin(), m_indices.begin() + 2);
		std::sort(m_indices.begin() + 2, m_indices.end());

		m_stiffness = (w[i1] + w[i2] + w[i3] + w[i4]) / 4.0f;
		
		m_stiffness = 1.000f;

	}
	void projectConstraint(unsigned int solvIt, std::vector<glm::vec3> &p, std::vector<glm::vec3> &s) const {
		unsigned int i1 = m_indices[0];
		unsigned int i2 = m_indices[1];
		unsigned int i3 = m_indices[2];
		unsigned int i4 = m_indices[3];
		
		glm::vec3 p2 = p[i2] - p[i1];
		glm::vec3 p3 = p[i3] - p[i1];
		glm::vec3 p4 = p[i4] - p[i1];

		glm::vec3 cp2p3 = glm::cross(p2, p3);
		glm::vec3 cp2p4 = glm::cross(p2, p4);

		glm::vec3 n1 = glm::normalize(cp2p3);
		glm::vec3 n2 = glm::normalize(cp2p4);
		float d = std::max(0.0f, std::min(glm::dot(n1, n2), 1.0f));

		glm::vec3 q3 = (glm::cross(p2, n2) + glm::cross(n1, p2) * d) /
			glm::length(cp2p3);
		glm::vec3 q4 = (glm::cross(p2, n1) + glm::cross(n2, p2) * d) /
			glm::length(cp2p4);
		glm::vec3 q2 = -(glm::cross(p3, n2) + glm::cross(n1, p3) * d) /
			glm::length(cp2p3) 
			- (glm::cross(p4, n1) + glm::cross(n2, p4) * d) /
			glm::length(cp2p4);
		glm::vec3 q1 = -q2 - q3 - q4;

		glm::vec3 s2 = s[i2] - s[i1];
		glm::vec3 s3 = s[i3] - s[i1];
		glm::vec3 s4 = s[i4] - s[i1];

		glm::vec3 sn1 = glm::normalize(glm::cross(s2, s3));
		glm::vec3 sn2 = glm::normalize(glm::cross(s2, s4));
		float rd = std::max(0.0f, std::min(glm::dot(sn1, sn2), 1.0f));

		float sum = glm::length2(q1) + glm::length2(q2) + glm::length2(q3) + glm::length2(q4);
		float coe = sum != 0 ? coe = -glm::sqrt(1 - d*d) * (acos(d) - acos(rd)) / sum : 0;

		float k = 1 - pow(1 - g_twVar.constraint_stiffness, 1 / (float) solvIt);
		coe *= k;
		
		p[i1] += coe * q1;
		p[i2] += coe * q2;
		p[i3] += coe * q3;
		p[i4] += coe * q4;
	}
};