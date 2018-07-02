#pragma once
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
class Collision {
public:
	Collision() {
		
		cameraBody = glm::vec3(0, 0, 0);

	}
	~Collision() {

	}
	void updateCameraBody(float x, float y, float z) {
		cameraBody = glm::vec3(x, y, z);
	}
	bool testCollision() {
		bool bx = false, by = false, bz = false;
		for (float i = cameraBody.x - 1; i < cameraBody.x + 1; i += 1) {
			if (i < SpringLocation.x + range && i > SpringLocation.x - range) {
				bx = true;
			}
		}
		for (float i = cameraBody.y - 1; i < cameraBody.y + 1; i += 1) {
			if (i < SpringLocation.y + range && i > SpringLocation.y - range) {
				by = true;
			}
		}
		for (float i = cameraBody.x - 1; i < cameraBody.x + 1; i += 1) {
			if (i < SpringLocation.z + range && i > SpringLocation.z - range) {
				bz = true;
			}
		}
		return bx && by && bz;
	}

private:
	glm::vec3 SpringLocation = glm::vec3(100.0f, 100.0f, 100.0f);
	float range = 2.0f;
	glm::vec3 cameraBody;
};