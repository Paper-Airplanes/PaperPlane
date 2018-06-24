#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include<glad\glad.h>
class Camera {
public:
	Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up, GLfloat speed) {
		cameraPos = position;
		cameraFront = front;
		cameraRight = glm::normalize(glm::cross(cameraFront, up));
		cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
		moveSpeed = speed;
		cameraYaw = -90.f;
		cameraPitch = 0.0f;
		//cameraFov = 45.0f;
	}
	void moveForward(float deltaTime) {
		float velocity = moveSpeed * deltaTime;
		cameraPos += cameraFront * velocity;
	}
	void moveBack(float deltaTime) {
		float velocity = moveSpeed * deltaTime;
		cameraPos -= cameraFront * velocity;
	}
	void moveRight(float deltaTime) {
		float velocity = moveSpeed * deltaTime;
		cameraPos += cameraRight * velocity;
	}

	void moveLeft(float deltaTime) {
		float velocity = moveSpeed * deltaTime;
		cameraPos -= cameraRight * velocity;
	}
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}
	void rotate(GLfloat const xoffset, GLfloat const yoffset) {
		cameraYaw += xoffset;
		cameraPitch += yoffset;
		if (cameraPitch > 89.0f)
			cameraPitch = 89.0f;
		if (cameraPitch < -89.0f)
			cameraPitch = -89.0f;
		glm::vec3 front;
		front.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
		front.y = sin(glm::radians(cameraPitch));
		front.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
		cameraFront = glm::normalize(front);
		cameraRight = glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));
		cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
	}
	glm::vec3 GetPosition() {
		return cameraPos;
	}
private:
	//GLfloat pfov, pratio, pnear, pfar;
	GLfloat moveSpeed;
	//float cameraFov;
	GLfloat cameraYaw, cameraPitch;
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraRight;
	glm::vec3 cameraUp;


};
#endif