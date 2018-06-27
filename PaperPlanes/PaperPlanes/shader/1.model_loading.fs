#version 330 core
out vec4 FragColor;


in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;


//uniform sampler2D texture_diffuse1;

void main()
{    
	float ambientStrength = 0.5;
	float specularStrength = 0.5;
	float diffuseStrength = 2.0;
	int Shininess = 128;

    vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);        //求出光源方向
	float diffuse = diffuseStrength * max(dot(norm, lightDir), 0.0);        //漫反射光照因子

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float specular = specularStrength * pow(max(dot(viewDir, reflectDir), 0.0), Shininess);    //镜面反射因子

    vec3 result = (ambientStrength + diffuse + specular) * lightColor * objectColor;
	FragColor = vec4(result, 1.0);

}