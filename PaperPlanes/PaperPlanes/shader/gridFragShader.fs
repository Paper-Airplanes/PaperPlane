#version 330 core

in vec3 Normal;
in vec2 texCoord;
in vec3 FragPos;
out vec4 FragColor;

uniform sampler2D ourTexture;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
	
	// init 
	vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
	float ambientStrength = 0.1f;
	float diffuseStrength = 0.5f;
	float specularStrength = 1.0f;
	int Shininess = 128;

	// calc
	vec3 ambient =	ambientStrength * lightColor;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = diffuseStrength * diff * lightColor;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), Shininess);
	vec3 specular = specularStrength * spec * lightColor;



	//vec3 objectColor = vec3(texture(ourTexture, texCoord));
	vec3 objectColor = vec3(1.0,1.0,1.0);
	vec3 result = (ambient + diffuse + specular) * objectColor;
	FragColor = vec4(result, 0.6f);
	//FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
