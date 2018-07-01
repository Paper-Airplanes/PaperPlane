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
    // 执行透视除法并且将坐标转换到[0,1]区间内
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // 在深度图中搜索特定(x, y)坐标下最小的深度值
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // 得到需要判断的点的深度值
    float currentDepth = projCoords.z;
    // 阴影偏移
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
    
    // z超过1.0的shadow都为0
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
	vec3 lightDir = normalize(lightPos - FragPos);        //求出光源方向
	float diffuse = diffuseStrength * max(dot(norm, lightDir), 0.0);        //漫反射光照因子

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float specular = specularStrength * pow(max(dot(viewDir, reflectDir), 0.0), Shininess);    //镜面反射因子

	// 阴影计算
    float shadow = ShadowCalculation(FragPosLightSpace);  
	vec3 result;
	if (hasTexture) {
		result = (ambientStrength + (1.0 - shadow) * (diffuse + specular)) * lightColor * vec3(texture(texture_diffuse0, texCoord));
	} else {
		result = (ambientStrength + (1.0 - shadow) * (diffuse + specular)) * lightColor * objectColor;
	}
	FragColor = vec4(result, 1.0);

}