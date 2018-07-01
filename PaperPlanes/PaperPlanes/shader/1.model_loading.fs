#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 texCoord;
in vec4 FragPosLightSpace;

uniform bool hasTexture;
uniform sampler2D shadowMap;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D texture_diffuse0;

//uniform sampler2D texture_diffuse1;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // ִ��͸�ӳ������ҽ�����ת����[0,1]������
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // �����ͼ�������ض�(x, y)��������С�����ֵ
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // �õ���Ҫ�жϵĵ�����ֵ
    float currentDepth = projCoords.z;
    // ��Ӱƫ��
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			if (currentDepth - bias > pcfDepth) {
				shadow += 1.0;
			}   
        }    
    }
    shadow /= 9.0;
    
    // z����1.0��shadow��Ϊ0
    if(projCoords.z > 1.0)
        shadow = 0.0;
    
    return shadow;
}

void main()
{    
	float ambientStrength = 0.5;
	float specularStrength = 0.5;
	float diffuseStrength = 1.0;
	int Shininess = 128;

    vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);        //�����Դ����
	float diffuse = diffuseStrength * max(dot(norm, lightDir), 0.0);        //�������������

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float specular = specularStrength * pow(max(dot(viewDir, reflectDir), 0.0), Shininess);    //���淴������

	// ��Ӱ����
    float shadow = ShadowCalculation(FragPosLightSpace);  
	vec3 result;
	if (hasTexture) {
		result = (ambientStrength + (1.0 - shadow) * (diffuse + specular)) * lightColor * vec3(texture(texture_diffuse0, texCoord));
	} else {
		result = (ambientStrength + (1.0 - shadow) * (diffuse + specular)) * lightColor * objectColor;
	}
	FragColor = vec4(result, 1.0);

}