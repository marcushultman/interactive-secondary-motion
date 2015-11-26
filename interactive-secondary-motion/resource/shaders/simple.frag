#version 330

// uniform sampler2D texture_diffuse1;

uniform bool enableWireframe;
uniform bool enableDistMag;

uniform vec3 lightPos; 
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

in vec2	a_texCoords;
in vec3	a_normal;
in vec3	a_fragPos;

in float a_distMag;

out vec4 fragmentColor;

void main() 
{
	if (enableWireframe){
		fragmentColor = vec4(0,0,0,1);
		return;
	}
	if (enableDistMag){
		float d = a_distMag * 6.0;
		if (d < .25){
			float a = d / .25;
			fragmentColor = vec4(0, a, 1, 1);
		}
		else if (d < .5){
			float a = (d - .25) / .25;
			fragmentColor = vec4(0, 1, 1-a, 1);
		}
		else if (d < .75){
			float a = (d - .5) / .25;
			fragmentColor = vec4(a, 1, 0, 1);
		}
		else {
			float a = (d - .75) / .25;
			fragmentColor = vec4(1, 1-a, 0, 1);
		}
		return;
	}


	// Ambient
    float ambientStrength = 0.4f;
    vec3 ambient = ambientStrength * lightColor;
  	
    // Diffuse 
    vec3 norm = normalize(a_normal);
    vec3 lightDir = normalize(lightPos - a_fragPos);

    float diff1 = max(dot(norm, lightDir), 0.0);
	float diff2 = max(dot(norm, -lightDir), 0.0);

    vec3 diffuse1 = diff1 * vec3(1, .8, .8) * lightColor;
	vec3 diffuse2 = diff2 * vec3(.4, .4, .6) * lightColor;

    
    // Specular
    float specularStrength = 0.1f;
    vec3 viewDir = normalize(viewPos - a_fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + (diffuse1 + diffuse2) + specular) * objectColor;

	fragmentColor = vec4(result, 1.0f);
	//fragmentColor = 0.5 * vec4(result, 1.0f) + 0.5 * a_color; // vec4(texture(texture_diffuse1, a_texCoords));
}