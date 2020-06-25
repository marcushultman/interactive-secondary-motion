#version 330

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

layout (location = 3) in ivec4 boneIds;
layout (location = 4) in vec4 boneWeights;

layout (location = 5) in float distMag;

out vec2 a_texCoords;
out vec3 a_normal;
out vec3 a_fragPos;

out float a_distMag;

void main() 
{
	gl_Position = projection * view * vec4(position, 1.0f);
	
	a_texCoords = texCoords;
	a_normal = normal;		//a_normal = mat3(transpose(inverse(model))) * normal;
	a_fragPos = position;	//a_fragPos = vec3(model * vec4(position, 1.0f));

	a_distMag = distMag;
}