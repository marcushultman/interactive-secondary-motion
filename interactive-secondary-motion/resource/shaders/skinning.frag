#version 330

uniform sampler2D texture_diffuse1;

uniform vec3 lightPos; 
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

in vec2 TexCoords0;
in vec3 Normal0;
in vec3 WorldPos0;

out vec4 fragmentColor;

void main() 
{
	// Ambient
    float ambientStrength = 0.2f;
    vec3 ambient = ambientStrength * lightColor;
  	
    // Diffuse 
    vec3 norm = normalize(Normal0);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - WorldPos0);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    
    fragmentColor = vec4(result, 1.0f);
    //fragmentColor = 0.5 * vec4(result, 1.0f) + 0.5 * Color; // vec4(texture(texture_diffuse1, TexCoords));
}