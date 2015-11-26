#include "ModelViewProjectionShader.h"


ModelViewProjectionShader::ModelViewProjectionShader(const GLchar* VSPath, const GLchar* FSPath)
	: Shader(VSPath, FSPath)
{
}


ModelViewProjectionShader::~ModelViewProjectionShader()
{
}


void ModelViewProjectionShader::setModelMatrix(const glm::mat4& model) const{
	glUseProgram(m_program);
	glUniformMatrix4fv(glGetUniformLocation(m_program, "model"),
		1, GL_FALSE, value_ptr(model));
}
void ModelViewProjectionShader::setViewMatrix(const glm::mat4& view) const{
	glUseProgram(m_program);
	glUniformMatrix4fv(glGetUniformLocation(m_program, "view"),
		1, GL_FALSE, value_ptr(view));
}
void ModelViewProjectionShader::setProjectionMatrix(const glm::mat4& proj) const{
	glUseProgram(m_program);
	glUniformMatrix4fv(glGetUniformLocation(m_program, "projection"),
		1, GL_FALSE, value_ptr(proj));
}