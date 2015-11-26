#version 330

uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec2 position;

void main() 
{
	gl_Position = projection * view * vec4(position.x, -0.5f, position.y,  1.0f);
}