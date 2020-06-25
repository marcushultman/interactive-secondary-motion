#version 330

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 a_normal;

void main() 
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	a_normal = normal;
}