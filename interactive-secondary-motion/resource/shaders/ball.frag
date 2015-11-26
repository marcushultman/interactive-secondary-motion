#version 330

in vec3 a_normal;

out vec4 fragmentColor;

void main() 
{
    vec3 norm = normalize(a_normal);
    float diff = 0.4f + 0.6f * max(dot(norm, vec3(1.f)), 0.0f);
    
	fragmentColor = vec4(vec3(diff), 1.0f);
}