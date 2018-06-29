#include <glad\glad.h>
#include <glfw\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include "Wave.h"
#include "MyWindow.h"

using namespace std;

#define INIT_HEIGHT 900
#define INIT_WIDTH 900

#define WATER_WIDTH 0.5
#define WATER_HEIGHT 0.5

int main() {

	GLFWwindow* window = NULL;

	float waveWidth = 0.5;
	float waveHeight = 0.5;

	//  背景颜色
	float clearColor[4] = { 0.5f, 0.5f, 0.5f, 0.5f };

	//  网格颜色
	glm::vec3 gridCenter = { 0.25, 0.25, 0.0f };

	//  矩阵
	glm::mat4 identity(1);
	glm::mat4 model(1);
	glm::mat4 view(1);
	glm::mat4 projection(1);

	//  光源、视角，上向量
	glm::vec3 lightPos = { 0.25, 0.25f, 1.0f };
	glm::vec3 viewPos = { 0.25, 0.25f, 1.0f };
	glm::vec3 upVec = { 0.0f, 1.0f, 0.0f };

	//  前
	float nearPlane = 0.01f, farPlane = 100.0f, time = 0.0f;

	//  初始化model, view, projection矩阵
	//model = glm::translate(identity, gridCenter);
	view = glm::lookAt(viewPos, gridCenter, upVec);
	projection = glm::perspective(glm::radians(60.0f), 1.0f, nearPlane, farPlane);

	//  创建窗口
	if (!myCreateWindow(window, "Wave", INIT_WIDTH, INIT_HEIGHT)) {
		return -1;
	}

	string texturePath = "D:/Projects/VS-Projects/Computer-Graphics-02/Final-Project/resources/wave.jpg";
	Wave myWave(waveWidth, waveHeight, model, view, projection, lightPos, viewPos, texturePath);

	glClear(GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_DEPTH_TEST);
	glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

	while (!glfwWindowShouldClose(window)) {
		myProcessInput(window);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT);

		myWave.show();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}