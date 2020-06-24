/*
* Copyright (c) 2015, Marcus Hultman
*/
#pragma once

#include <GL/glew.h>

#include <assimp/scene.h>

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include <vector>
#include <map>
#include <set>

#include "CollisionConstraint.h"
#include "DistanceConstraint.h"
#include "BendingConstraint.h"

#include "Shader.h"
#include "RigidSphere.h"

struct BoneData
{
	glm::ivec4 indices;
	glm::vec4 weights;
};

#define EB_INDEX		0
#define VB_POSITION		1
#define VB_NORMAL		2
#define VB_TEXCOORD		3
#define VB_BONE_DATA	4
#define VB_DIST_MAG		5
#define NUM_BUFFERS		6

class Mesh {
public:
	Mesh(const aiScene*, const aiMesh*);
	~Mesh();

	void loadBoneData(std::map<std::string, unsigned int> boneIndicies);
	void reset(std::vector<glm::mat4> &transforms);

	void update(double elapsedTime, std::vector<glm::mat4> &transforms);
	void draw(const Shader* shader, const bool secondaryEffects);

	unsigned int raycastVertex(glm::vec3 origin, glm::vec3 dir) const;

	glm::vec3 getVertexPosition(unsigned int index) const;
	void setVertexPosition(unsigned int index, glm::vec3 position);

private:
	// Scene data
	const aiScene* m_pScene;
	const aiMesh* m_pMesh;

	// Animation data
	std::vector<glm::mat4> m_boneOffsets;

	// Vertex data
	unsigned int m_numVertices, m_numIndices;
	std::vector<glm::vec3> m_restPositions, m_skinPositions;	// Layer 1
	std::vector<glm::vec3> m_pbdPositions, m_pbdVelocities;		// Layer 2

	std::vector<float> m_pbdWeights;
	std::vector<BoneData> m_boneData;
	
	std::set<Constraint*, Constraint::comparator> m_constraints;
	
	// OpenGL objects
	GLuint m_VAO;
	GLuint m_buffers[NUM_BUFFERS];

	void processMesh();
	void createConstraints();
};