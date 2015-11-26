#include "Mesh.h"

#include "TwVars.h"
extern TwVars g_twVar;

extern std::pair<unsigned int, glm::vec3> *g_drag;
extern RigidSphere *g_ball;


Mesh::Mesh(const aiScene* scene, const aiMesh* mesh)
{
	m_pScene = scene;
	m_pMesh = mesh;
	processMesh();
}

Mesh::~Mesh()
{
}


void Mesh::loadBoneData(std::map<std::string, unsigned int> boneIndicies)
{
	m_boneOffsets.resize(boneIndicies.size());
	m_boneData.resize(m_numVertices);

	for (unsigned int j = 0; j < m_pMesh->mNumBones; j++) {
		aiBone* bone = m_pMesh->mBones[j];
		int index = boneIndicies[bone->mName.data];

		memcpy(&m_boneOffsets[index],
			&bone->mOffsetMatrix.Transpose(), sizeof(glm::mat4));

		// Set vertex bone data
		for (unsigned int i = 0; i < m_pMesh->mBones[j]->mNumWeights; i++){
			aiVertexWeight vert_weight = m_pMesh->mBones[j]->mWeights[i];
			BoneData* data = &m_boneData[vert_weight.mVertexId];
			for (unsigned int k = 0; k < 4; k++){
				if (data->weights[k] > 0)
					continue;
				data->indices[k] = index;
				data->weights[k] = vert_weight.mWeight;
				break;
			}
		}
	}

	// Upload vertex bone data
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VB_BONE_DATA]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(BoneData) * m_numVertices,
		&m_boneData[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(BoneData), 0);

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(BoneData),
		(GLvoid*) offsetof(BoneData, weights));

	glBindVertexArray(0);

}

void Mesh::reset(std::vector<glm::mat4> &transforms)
{
	std::vector<glm::mat4> finalTransforms(transforms.size());
	for (unsigned int j = 0; j < transforms.size(); j++){
		finalTransforms[j] = transforms[j] * m_boneOffsets[j];
	}	
	for (unsigned int i = 0; i < m_numVertices; i++){
		BoneData* data = &m_boneData[i];
		glm::mat4 boneTransform;
		if (data->weights[0] > 0){
			boneTransform = data->weights[0] * finalTransforms[data->indices[0]];
			boneTransform += data->weights[1] * finalTransforms[data->indices[1]];
			boneTransform += data->weights[2] * finalTransforms[data->indices[2]];
			boneTransform += data->weights[3] * finalTransforms[data->indices[3]];
		}
		m_pbdPositions[i] = glm::vec3(boneTransform * glm::vec4(m_restPositions[i], 1));
	}
}


void Mesh::update(double elapsedTime, std::vector<glm::mat4> &transforms)
{
	// Skinning
	if (m_boneData.size() > 0){
		std::vector<glm::mat4> finalTransforms(transforms.size());
		for (unsigned int j = 0; j < transforms.size(); j++){
			finalTransforms[j] = transforms[j] * m_boneOffsets[j];
		}	
		for (unsigned int i = 0; i < m_numVertices; i++){
			BoneData* data = &m_boneData[i];
			glm::mat4 boneTransform;
			if (data->weights[0] > 0){
				boneTransform = data->weights[0] * finalTransforms[data->indices[0]];
				boneTransform += data->weights[1] * finalTransforms[data->indices[1]];
				boneTransform += data->weights[2] * finalTransforms[data->indices[2]];
				boneTransform += data->weights[3] * finalTransforms[data->indices[3]];
			}
			m_skinPositions[i] = glm::vec3(boneTransform * glm::vec4(m_restPositions[i], 1));
		}
	}
	else{
		for (unsigned int i = 0; i < m_numVertices; i++){
			m_skinPositions[i] = m_restPositions[i];
		}	
	}

	// Prediceted positions
	std::vector<glm::vec3> p(m_numVertices);

	// Symplectic Euler
	for (unsigned int i = 0; i < m_numVertices; i++){
		m_pbdVelocities[i] += (float) elapsedTime * glm::vec3(0, -g_twVar.sim_gravity, 0);
		p[i] = m_pbdPositions[i] + (float) elapsedTime * m_pbdVelocities[i];
	}

	// genCollisionConstraints
	std::set<Constraint*, Constraint::comparator> collisionConstraints;

	// Ball collision
	for (unsigned int i = 0; i < m_numVertices; i++){
		glm::vec3 diff = p[i] - g_ball->getPosition();
		float diffDist2 = glm::dot(diff, diff);
		if (diffDist2 < g_ball->getRadius2()){
			glm::vec3 diffNorm = glm::normalize(diff);
			collisionConstraints.insert(new CollisionConstraint(i,
				g_ball->getPosition() + diffNorm * g_ball->getRadius()));
		}
	}
	// Drag
	if (g_drag && g_drag->first < p.size()){
		collisionConstraints.insert(new CollisionConstraint(g_drag->first,
			g_drag->second));
	}

	unsigned int solverIteration = 4;
	for (unsigned int i = 0; i < solverIteration; i++) {
		// projectConstraints(C1,...,CM+Mcoll, p1,...,pN, s1,...,sN)
		for (Constraint* c : m_constraints){
			c->projectConstraint(solverIteration, p, m_skinPositions);
		}
		for (Constraint* c : collisionConstraints){
			c->projectConstraint(solverIteration, p, m_skinPositions);
		}
	}
	// Clean up collision constraints
	for (Constraint* c : collisionConstraints){
		delete c;
	}	

	// Layer Blending Function
	for (unsigned int i = 0; i < m_numVertices; i++){
		float mult = g_twVar.blend_useWeights ? m_pbdWeights[i] : g_twVar.blend_multiplier;
		p[i] = (1 - mult) * m_skinPositions[i] + mult * p[i];
	}

	// Calculate velocities from updated positions
	for (unsigned int i = 0; i < m_numVertices; i++) {
		m_pbdVelocities[i] = (p[i] - m_pbdPositions[i]) / (float) elapsedTime;
		m_pbdPositions[i] = p[i];
	}
}

void Mesh::draw(const Shader* shader, const bool secondaryEffects)
{
	glBindVertexArray(m_VAO);

	// Upload positions
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VB_POSITION]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * m_numVertices,
		secondaryEffects ? &m_pbdPositions[0] : &m_skinPositions[0]);

	// For wireframe, render with offset
	if (g_twVar.render_wireframe)
		glEnable(GL_POLYGON_OFFSET_FILL);

	if (g_twVar.render_fill){
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (g_twVar.render_distMag){
			
			//glUniform1i(glGetUniformLocation(shader->getProgram(), "cooldata"), &cooldata);

			std::vector<float> dist_mag(m_numVertices);
			for (unsigned int i = 0; i < m_numVertices; i++) {
				dist_mag[i] = glm::length(m_skinPositions[i] - m_pbdPositions[i]);
			}
			glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VB_DIST_MAG]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * m_numVertices, &dist_mag[0]);

			glUniform1i(glGetUniformLocation(shader->getProgram(), "enableDistMag"), true);
			glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0);
			glUniform1i(glGetUniformLocation(shader->getProgram(), "enableDistMag"), false);

		}
		else {
			glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0);
		}
	}
	else{
		// No options
	}

	// Over lay wireframe
	if (g_twVar.render_wireframe){
		glDisable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glUniform1i(glGetUniformLocation(shader->getProgram(), "enableWireframe"), true);
		glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glUniform1i(glGetUniformLocation(shader->getProgram(), "enableWireframe"), false);
	}

	glBindVertexArray(0);
}


unsigned int Mesh::raycastVertex(glm::vec3 origin, glm::vec3 dir) const
{
	int index = -1;
	float minDist = FLT_MAX;
	for (unsigned int i = 0; i < m_numVertices; i++){
		glm::vec3 a = m_pbdPositions[i] - origin;
		glm::vec3 b = dot(a, dir) * dir;
		if (glm::length2(a - b) < 0.002f){
			float d = glm::length2(b);
			if (d < minDist){
				minDist = d;
				index = i;
			}
		}
	}
	return index;
}

glm::vec3 Mesh::getVertexPosition(unsigned int index) const
{
	return m_pbdPositions[index];
}

void Mesh::setVertexPosition(unsigned int index, glm::vec3 position)
{
	m_pbdPositions[index] = position;
}


void Mesh::processMesh()
{
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	glGenBuffers(NUM_BUFFERS, m_buffers);

	m_numVertices = m_pMesh->mNumVertices;

	m_restPositions.resize(m_numVertices);
	m_skinPositions.resize(m_numVertices);

	m_pbdPositions.resize(m_numVertices);
	m_pbdVelocities.resize(m_numVertices);
	m_pbdWeights.resize(m_numVertices);

	std::vector<glm::vec3> normals(m_numVertices);
	std::vector<glm::vec2> texCoords(m_numVertices);
	std::vector<unsigned int> indices;

	// Load vertex data
	for (unsigned int i = 0; i < m_numVertices; i++) {
		// Positions
		memcpy(&m_restPositions[i], &m_pMesh->mVertices[i], sizeof(glm::vec3));
		// Normals
		memcpy(&normals[i], &m_pMesh->mNormals[i], sizeof(glm::vec3));
		// TexCoords
		if (m_pMesh->mTextureCoords[0]){
			memcpy(&texCoords[i], &m_pMesh->mTextureCoords[0][i], sizeof(glm::vec2));
		}
		// Skin softness
		memcpy(&m_pbdWeights[i], &m_pMesh->mColors[0][i].r, sizeof(float));
	}
	// Load index data
	for (unsigned int i = 0; i < m_pMesh->mNumFaces; i++) {
		aiFace* face = &m_pMesh->mFaces[i];
		// Retrieve all indices of the face and store them in the indices vector
		for (unsigned int i = 0; i < face->mNumIndices; i++){
			indices.push_back(face->mIndices[i]);
		}
	}

	createConstraints();

	printf("Mesh loaded. N: %d\tM: %d\n", m_numVertices, m_constraints.size());

	// Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers[EB_INDEX]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * (m_numIndices = indices.size()),
		&indices[0], GL_STATIC_DRAW);

	// Vertex positions
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VB_POSITION]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_numVertices,
		&m_restPositions[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Vertex normals
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VB_NORMAL]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_numVertices,
		&normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Vertex texture coordinates
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VB_TEXCOORD]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * m_numVertices,
		&texCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);


	// Distortion magnitude
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers[VB_DIST_MAG]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_numVertices, 0, GL_STATIC_DRAW);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, 0, 0);


	glBindVertexArray(0);
}

void Mesh::createConstraints()
{
	unsigned int c[3][3]{ { 0, 1, 2 }, { 0, 2, 1 }, { 1, 2, 0 } };
	std::map<unsigned int, std::map<unsigned int,std::vector<unsigned int>>>adjFaces;

	for (unsigned int i = 0; i < m_pMesh->mNumFaces; i++) {
		aiFace* face = &m_pMesh->mFaces[i];
		// Add 3 distance constraints - one for each edge in this face
		for (unsigned int i = 0; i < 3; i++){
			std::vector<unsigned int> ix = { face->mIndices[c[i][0]], face->mIndices[c[i][1]] };
			std::sort(ix.begin(), ix.end());
			m_constraints.insert(new DistanceConstraint(
				ix[0], ix[1], m_pbdWeights));
			adjFaces[ix[0]][ix[1]].push_back(face->mIndices[c[i][2]]);
		}
	}
	for (std::pair<unsigned int, std::map<unsigned int, std::vector<unsigned int>>> i : adjFaces){
		for (std::pair<unsigned int, std::vector<unsigned int>> j : i.second){
			if (j.second.size() > 1){
				m_constraints.insert(new BendingConstraint(
					i.first, j.first, j.second[0], j.second[1], m_pbdWeights));
			}
		}
	}
}
