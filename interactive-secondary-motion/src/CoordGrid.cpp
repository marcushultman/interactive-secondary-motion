#include "CoordGrid.h"


CoordGrid::CoordGrid()
{
	m_size = 0.1f;
	m_subDivisions = 10;
	createGrid();
	m_pShader = new ModelViewProjectionShader("resource/shaders/grid.vert", "resource/shaders/grid.frag");
}


CoordGrid::~CoordGrid()
{
	delete m_pShader;
	m_pShader = 0;
}

void CoordGrid::draw(const glm::mat4 &view, const glm::mat4 &proj)
{
	m_pShader->setViewMatrix(view);
	m_pShader->setProjectionMatrix(proj);

	glBindVertexArray(m_VAO);
	glDrawElements(GL_LINES, m_numIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void CoordGrid::createGrid()
{
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	GLuint buffers[2];
	glGenBuffers(2, buffers);

	std::vector<glm::vec2> vertices(4 * m_subDivisions);
	std::vector<glm::vec2>::iterator v = vertices.begin();
	/*
	0 2 4
	. . .
	6 .   .  7
	. . .
	1 3 5


	-1 0 1

	*/


	int start = m_subDivisions / 2;
	for (int x = -start; x <= start; x++) {
		*v++ = m_size * glm::vec2(x, -start);
		*v++ = m_size * glm::vec2(x, start);
	}
	for (int y = -start + 1; y < start; y++) {
		*v++ = m_size * glm::vec2(-start, y);
		*v++ = m_size * glm::vec2(start, y);
	}

	std::vector<unsigned int> indices(4 * (m_subDivisions + 1));
	std::vector<unsigned int>::iterator i = indices.begin();
	for (unsigned int x = 0; x < 4 * m_subDivisions; x++) {
		*i++ = x;
	}
	*i++ = 0;
	*i++ = 2 * m_subDivisions;
	*i++ = *(i - 2) + 1;
	*i++ = *(i - 2) + 1;

	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * (m_numIndices = indices.size()),
		&indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}