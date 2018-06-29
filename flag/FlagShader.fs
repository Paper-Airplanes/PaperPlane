#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;

uniform sampler2D faceTexture;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
	// init 
	vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
	float ambientStrength = 0.1f;
	float diffuseStrength = 1.0f;
	float specularStrength = 0.0f;
	int Shininess = 32;

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

	//FragColor = texture(faceTexture, TexCoord);
	vec3 objectColor = vec3(texture(faceTexture, TexCoord));
	vec3 result = (ambient + diffuse + specular) * objectColor;
	FragColor = vec4(result, 1.0f);
}
