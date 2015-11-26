/*
* Copyright © 2015, Marcus Hultman
*/
#pragma once

#include <glm/glm.hpp>
#include <vector>

class Constraint
{
public:
	virtual void projectConstraint(unsigned int solvIt, std::vector<glm::vec3> &p,
		std::vector<glm::vec3> &s) const = 0;
	
	// For this basic implementation, sort out constraints for the same indices
	struct comparator {
		bool operator () (Constraint* c1, Constraint* c2) {
			return c1->m_indices < c2->m_indices;
		}
	};
protected:
	float m_stiffness;
	std::vector<unsigned int> m_indices;
	bool m_equality;
};

