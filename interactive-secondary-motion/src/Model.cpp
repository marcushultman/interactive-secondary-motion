#include "Model.h"

#include "TwVars.h"
extern TwVars g_twVar;

extern RigidSphere *g_ball;


Model::Model(const char* file)
{
	// Load model data and meshes
	if (!loadModel(file))
		return;

	// Load animation data, setup bone data for the loaded meshes
	loadAnimation();

	// Load shader
	m_pShader = new ModelViewProjectionShader(
		"resource/shaders/simple.vert", "resource/shaders/simple.frag");

	// Set light properties
	glUseProgram(m_pShader->getProgram());
	glUniform3f(glGetUniformLocation(m_pShader->getProgram(), "objectColor"),
		1.0f, 0.88f, 0.77f);
	glUniform3f(glGetUniformLocation(m_pShader->getProgram(), "lightColor"),
		1.0f, 1.0f, 1.0f);
	glUniform3f(glGetUniformLocation(m_pShader->getProgram(), "lightPos"),
		20, 20, 20);
}

Model::~Model()
{
	delete m_pShader;
	m_pShader = 0;
}


void Model::loadAnimation(const char* animationName)
{
	// Load animation by name of first if no specified.
	for (unsigned int i = 0; i < m_pScene->mNumAnimations; i++){
		if (!animationName || m_pScene->mAnimations[i]->mName.data == animationName){
			m_pAnimation = m_pScene->mAnimations[i];
			break;
		}
	}

	if (!m_pAnimation)
		return;

	// Map bone names to indicies
	m_boneIndicies.clear();
	unsigned int numBones = m_pAnimation->mNumChannels;
	for (unsigned int i = 0; i < numBones; i++){
		m_boneIndicies[m_pAnimation->mChannels[i]->mNodeName.data] = i;
	}
	// Load bone data
	for (unsigned int i = 0; i < m_meshes.size(); i++){
		m_meshes[i].loadBoneData(m_boneIndicies);
	}

	// Initialize bone transforms
	m_boneTransforms.resize(numBones);
	resetAnimation();
}

void Model::resetAnimation(){
	updateBoneTransforms(m_animationTime = 0, m_pScene->mRootNode, glm::mat4());
	for (unsigned int i = 0; i < m_meshes.size(); i++){
		m_meshes[i].reset(m_boneTransforms);
	}
}


void Model::update(double elapsedTime)
{
	if (m_pAnimation){
		if (m_animate){
			m_animationTime += g_twVar.anim_speed* elapsedTime *
				(m_pAnimation->mTicksPerSecond > 0 ? m_pAnimation->mTicksPerSecond : 1);
			// Stop animation when finished
			if (m_animationTime >= m_pAnimation->mDuration){
				m_animationTime = m_pAnimation->mDuration;
				m_animate = false;
			}
		}
		updateBoneTransforms(m_animationTime, m_pScene->mRootNode, glm::mat4());
	}

	for (unsigned int i = 0; i < m_meshes.size(); i++){
		m_meshes[i].update(elapsedTime, m_boneTransforms);
	}	
}


void Model::draw(const glm::mat4& view, const glm::mat4& proj,
	const glm::vec3& cameraPosition, const bool secondaryEffects)
{
	m_pShader->setViewMatrix(view);
	m_pShader->setProjectionMatrix(proj);

	glUniform3fv(glGetUniformLocation(m_pShader->getProgram(), "viewPos"),
		1, value_ptr(cameraPosition));

	for (unsigned int i = 0; i < m_meshes.size(); i++){
		m_meshes[i].draw(m_pShader, secondaryEffects);
	}	
}

const unsigned int Model::getNumMeshes(){
	return m_meshes.size();
}

const Mesh* Model::getMesh(unsigned int index)
{
	if (index < 0 || index >= m_meshes.size())
		return nullptr;
	return &m_meshes[index];
}



bool Model::loadModel(std::string path)
{
	m_importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS,
		aiComponent_CAMERAS |
		aiComponent_LIGHTS |
		aiComponent_TANGENTS_AND_BITANGENTS |
		aiComponent_NORMALS);
	m_pScene = m_importer.ReadFile(path,
		aiProcess_RemoveComponent |
		aiProcess_JoinIdenticalVertices |
		aiProcess_GenSmoothNormals |
		aiProcess_Triangulate |
		aiProcess_FlipUVs);
	// Check for errors
	if (!m_pScene || m_pScene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !m_pScene->mRootNode){
		printf("Assimp error: %s\n", m_importer.GetErrorString());
		return false;
	}
	// TODO: Use for texture loading
	//directory_ = path.substr(0, path.find_last_of('/'));

	// TODO: Global positioning, store the inverse global position
	//memcpy(&globalnverseTransform_, &scene->mRootNode->mTransformation.Inverse(), sizeof(mat4));

	// Process node hierarchy
	findMeshes(m_pScene->mRootNode);

	return true;
}

void Model::findMeshes(const aiNode* node)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++){
		m_meshes.push_back(Mesh(m_pScene, m_pScene->mMeshes[node->mMeshes[i]]));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++){
		findMeshes(node->mChildren[i]);
	}	
}

void Model::updateBoneTransforms(double t, const aiNode* node, const glm::mat4& parentTransform)
{
	// Find animation channel
	aiNodeAnim* animationChannel = NULL;
	std::string nodeName = node->mName.data;
	for (unsigned int i = 0; i < m_pAnimation->mNumChannels; i++){
		if (m_pAnimation->mChannels[i]->mNodeName.data == nodeName){
			animationChannel = m_pAnimation->mChannels[i];
			break;
		}
	}

	// Calculate transform
	glm::mat4 nodeTransform;
	if (animationChannel) {
		// Interpolate keys
		aiVectorKey* tStartKey = animationChannel->mPositionKeys;
		aiVectorKey* tEndKey = animationChannel->mPositionKeys + 1;
		while (tStartKey->mTime > t || t > tEndKey->mTime){
			tStartKey++; tEndKey++;
		}
		aiMatrix4x4 aiPosM;
		float tFactor = (float) ((t - tStartKey->mTime) /
			(tEndKey->mTime - tStartKey->mTime));
		aiVector3D aiPos = tStartKey->mValue + tFactor * (tEndKey->mValue - tStartKey->mValue);
		aiMatrix4x4::Translation(aiPos, aiPosM);

		aiQuatKey* rStartKey = animationChannel->mRotationKeys;
		aiQuatKey* rEndKey = animationChannel->mRotationKeys + 1;
		while (rStartKey->mTime > t || t > rEndKey->mTime){
			rStartKey++; rEndKey++;
		}
		aiQuaternion aiRot;
		float rFactor = (float) ((t - rStartKey->mTime) /
			(rEndKey->mTime - rStartKey->mTime));
		aiQuaternion::Interpolate(aiRot, rStartKey->mValue, rEndKey->mValue, rFactor);

		// Combine the above transformations
		aiMatrix4x4 nodeTransformM = aiPosM * aiMatrix4x4(aiRot.GetMatrix());

		memcpy(&nodeTransform, &nodeTransformM, sizeof(glm::mat4));
	}
	else{
		memcpy(&nodeTransform, &node->mTransformation, sizeof(glm::mat4));
	}
	glm::mat4 globalTransform = parentTransform * transpose(nodeTransform);

	// Set bone transfrom
	if (m_boneIndicies.find(nodeName) != m_boneIndicies.end()) {
		unsigned int index = m_boneIndicies[nodeName];
		m_boneTransforms[index] = globalTransform;

		// Start animation on bone collision
		glm::vec3 bonePos = glm::vec3(globalTransform[3]);
		if (glm::distance2(bonePos, g_ball->getPosition()) < g_ball->getRadius2() && 
			g_ball->startCollision()){
			m_animationTime = 0;
			m_animate = true;
		}

	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		updateBoneTransforms(t, node->mChildren[i], globalTransform);
	}
}