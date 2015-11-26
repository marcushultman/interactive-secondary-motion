/*
* Copyright © 2015, Marcus Hultman
*/
#pragma once

#include <GL/glew.h>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <map>
#include <tuple>

#include "ModelViewProjectionShader.h"
#include "Mesh.h"
#include "RigidSphere.h"


class Model
{
public:
	Model(const char* file);
	~Model();

	void loadAnimation(const char* animationName = 0);
	void resetAnimation();

	void update(double elapsedTime);

	void draw(const glm::mat4& view, const glm::mat4& proj,
		const glm::vec3& cameraPosition, const bool secondaryEffects = true);


	const unsigned int getNumMeshes();
	const Mesh* getMesh(unsigned int index);

private:
	// Model data
	Assimp::Importer m_importer;
	const aiScene* m_pScene;
	const ModelViewProjectionShader* m_pShader;

	std::vector<Mesh> m_meshes;
	

	bool m_animate;
	aiAnimation* m_pAnimation;
	double m_animationTime = 0;

	std::map<std::string, unsigned int> m_boneIndicies;
	std::vector<glm::mat4> m_boneTransforms;


	bool loadModel(std::string path);
	void findMeshes(const aiNode* node);

	void updateBoneTransforms(double t, const aiNode* node, const glm::mat4& parentTransform);
};