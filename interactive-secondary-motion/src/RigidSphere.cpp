#include "RigidSphere.h"


RigidSphere::RigidSphere()
{
	m_radius = 0.38f;
	m_radius2 = m_radius * m_radius;
	createMesh(m_radius, 18, 18);
	m_pShader = new ModelViewProjectionShader("resource/shaders/ball.vert", "resource/shaders/ball.frag");
	reset(false);
}


RigidSphere::~RigidSphere()
{
	delete m_pShader;
	m_pShader = 0;
}

void RigidSphere::reset()
{
	reset(true);
}
void RigidSphere::reset(bool startScenario)
{
	m_hasCollided = false;

	m_prevPosition = m_position = glm::vec3(-1.0f, 0.0f, 0.215f);
	if (startScenario){
		m_prevPosition = glm::vec3(-1.008f, 0.0f, 0.215f);
	}	
}

void RigidSphere::update(double elapsedTime)
{
	glm::vec3 v = m_position - m_prevPosition;
	m_prevPosition = m_position;
	m_position += v;

	//position_ += vec3(0, -0.001f * var_GRAVITY, 0);

	if (m_position.y < -0.5f + m_radius){
		m_position.y = -0.5f + m_radius;
		m_prevPosition.y = (m_position.y + v.y) * 1.001f;
	}
}

void RigidSphere::draw(const glm::mat4 &view, const glm::mat4 &proj)
{
	glm::mat4 model(1.0);
	model[3] = glm::vec4(m_position, 1.0);
	draw(view, proj, model);
}

void RigidSphere::draw(const glm::mat4 &view, const glm::mat4 &proj,
	const glm::mat4 &model)
{
	m_pShader->setModelMatrix(model);
	m_pShader->setViewMatrix(view);
	m_pShader->setProjectionMatrix(proj);

	glBindVertexArray(m_VAO);
	glDrawElements(GL_QUADS, m_numIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

glm::vec3 RigidSphere::getPosition()
{
	return m_position;
}
float RigidSphere::getRadius()
{
	return m_radius;
}
float RigidSphere::getRadius2()
{
	return m_radius2;
}


bool RigidSphere::startCollision()
{
	if (m_hasCollided){
		return false;
	}
	m_hasCollided = true;

	glm::vec3 v = m_position - m_prevPosition;
	m_prevPosition.x = m_position.x + v.x * 0.5f;
	m_prevPosition.y -= 0.00001f;
	return true;
}


void RigidSphere::createMesh(float radius, unsigned int rings, unsigned int sectors)
{
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	GLuint buffers[2];
	glGenBuffers(2, buffers);

	unsigned int r, s;

	std::vector<glm::vec3> vertices(rings * sectors);
	std::vector<glm::vec3> normals(rings * sectors);
	std::vector<glm::vec3>::iterator v = vertices.begin();
	std::vector<glm::vec3>::iterator n = normals.begin();

	float dx = (float) (2 * M_PI) / (float) (sectors - 1);
	float dy = (float) M_PI / (float) (rings - 1);
	float x, y;

	for (r = 0, y = 0; r < rings; r++, y += dy) for (s = 0, x = 0; s < sectors; s++, x += dx) {
		float sy = sin(y);
		*v = radius * glm::vec3(cos(x) * sy, cos(y), sin(x) * sy);
		*n++ = glm::normalize(*v++);
	}

	std::vector<unsigned int> indices(rings * sectors * 4);
	std::vector<unsigned int>::iterator i = indices.begin();
	for (r = 0; r < rings - 1; r++) for (s = 0; s < sectors - 1; s++) {
		*i++ = r * sectors + s;
		*i++ = r * sectors + (s + 1);
		*i++ = (r + 1) * sectors + (s + 1);
		*i++ = (r + 1) * sectors + s;
	}

	GLintptr vertSize = sizeof(glm::vec3) * rings * sectors;
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, 2 * vertSize, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertSize, &vertices[0]);
	glBufferSubData(GL_ARRAY_BUFFER, vertSize, vertSize, &normals[0]);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*) vertices.size());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * (m_numIndices = indices.size()),
		&indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}