#include "Shader.h"


Shader::Shader(const GLchar* VSPath, const GLchar* FSPath)
{
	// 1. Retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	try
	{
		// Open files
		std::ifstream vShaderFile(VSPath);
		std::ifstream fShaderFile(FSPath);
		std::stringstream vShaderStream, fShaderStream;
		// Read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// Convert stream into string
		vertexCode.assign(vShaderStream.str());
		fragmentCode.assign(fShaderStream.str());
	}
	catch (std::exception e)
	{
		printf("Shader error: files could not be read (%s)", VSPath);
	}
	// 2. Compile shaders
	GLuint vertex, fragment;
	GLint success;
	GLchar infoLog[512];
	// Vertex Shader
	{
		vertex = glCreateShader(GL_VERTEX_SHADER);
		const GLchar* src = vertexCode.c_str();
		glShaderSource(vertex, 1, &src, NULL);
		glCompileShader(vertex);
		// Print compile errors if any
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success){
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			printf("Shader error: vertex compilation failed. %s", infoLog);
		}
	}

	// Fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar* src = fragmentCode.c_str();
	glShaderSource(fragment, 1, &src, NULL);
	glCompileShader(fragment);
	// Print compile errors if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success){
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		printf("Shader error: fragment compilation failed. %s", infoLog);
	}
	// Shader Program
	m_program = glCreateProgram();
	glAttachShader(m_program, vertex);
	glAttachShader(m_program, fragment);
	glLinkProgram(m_program);
	// Print linking errors if any
	glGetProgramiv(m_program, GL_LINK_STATUS, &success);
	if (!success){
		glGetProgramInfoLog(m_program, 512, NULL, infoLog);
		printf("Shader error: linking failed. %s", infoLog);
	}
	// Delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}


Shader::~Shader()
{
	glDeleteProgram(m_program);
}

const GLuint Shader::getProgram() const
{
	return m_program;
}