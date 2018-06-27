#pragma once
#include<glm\glm\glm.hpp>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtx/norm.hpp>
using namespace glm;
class Light
{
public:
	vec3 lightPosition;
	mat4 lightProjection;
	mat4 lightView;
	mat4 lightSpaceMatrix;
	Light(vec3 pos) {
		lightPosition = pos;
		float near_plane = 1.0f, far_plane = 500.0f;
		lightProjection = glm::ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f, near_plane, far_plane);
		//lightProjection = glm::perspective(45.0f, 1.0f, 0.1f, 100.0f);
		lightView = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
	}
	~Light();

private:

};

