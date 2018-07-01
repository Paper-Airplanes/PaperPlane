#include <glad\glad.h>
#include <glfw\glfw3.h>
#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtc\type_ptr.hpp>
#include "Wave.h"
#include "MyWindow.h"

using namespace std;

#define INIT_HEIGHT 900
#define INIT_WIDTH 900

#define WATER_WIDTH 0.5
#define WATER_HEIGHT 0.5

int main() {

	GLFWwindow* window = NULL;

	float waveWidth = 0.6;
	float waveHeight = 0.4;

	//  ������ɫ
	float clearColor[4] = { 0.5f, 0.5f, 0.5f, 0.5f };

	//  ������ɫ
	glm::vec3 gridCenter = { 0.25, 0.25, 0.0f };

	//  ����
	glm::mat4 identity(1);
	glm::mat4 model(1);
	glm::mat4 view(1);
	glm::mat4 projection(1);

	//  ��Դ���ӽǣ�������
	glm::vec3 lightPos = { 0.25, 0.25f, 1.0f };
	glm::vec3 viewPos = { 0.25, 0.25f, 1.0f };
	glm::vec3 upVec = { 0.0f, 1.0f, 0.0f };

	//  ǰ
	float nearPlane = 0.01f, farPlane = 100.0f, time = 0.0f;

	//  ��ʼ��model, view, projection����
	//model = glm::translate(identity, gridCenter);
	view = glm::lookAt(viewPos, gridCenter, upVec);
	projection = glm::perspective(60.0f, 1.0f, nearPlane, farPlane);

	//  ��������
	if (!myCreateWindow(window, "Wave", INIT_WIDTH, INIT_HEIGHT)) {
		return -1;
	}
	Shader* waveShader = new Shader("./shader/gridVertexShader.vs", "./shader/gridFragShader.fs");
	
	string texturePath = "./resources/texture/water-texture-2.tga";
	Wave* wave = new Wave(0.6, 0.5, "./resources/texture/water-texture-2.tga");
	wave->prepare();
	//glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

	while (!glfwWindowShouldClose(window)) {
		myProcessInput(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glClear(GL_COLOR_BUFFER_BIT);
		//wave->prepare();

		float clearColor[4] = { 0.5f, 0.5f, 0.5f, 0.5f };

		//  ������ɫ
		glm::vec3 gridCenter = { 0.25, 0.25, 0.0f };

		//  ����
		glm::mat4 identity(1);
		glm::mat4 model(1);
		glm::mat4 view(1);
		glm::mat4 projection(1);

		//  ��Դ���ӽǣ�������
		glm::vec3 lightPos = { 0.25, 0.25f, 1.0f };
		glm::vec3 viewPos = { 0.25, 0.25f, 1.0f };
		glm::vec3 upVec = { 0.0f, 1.0f, 0.0f };

		//  ǰ
		float nearPlane = 0.01f, farPlane = 100.0f, time = 0.0f;

		//  ��ʼ��model, view, projection����
		//model = glm::translate(identity, gridCenter);
		view = glm::lookAt(viewPos, gridCenter, upVec);
		projection = glm::perspective(60.0f, 1.0f, nearPlane, farPlane);
		waveShader->use();
		waveShader->setVec3("lightPos", lightPos);
		//glm::mat4 model;
		model = glm::scale(model, glm::vec3(10.02f, 10.02f, 10.02f));
		waveShader->setMat4("model", model);
		waveShader->setMat4("projection", projection);
		waveShader->setVec3("viewPos", viewPos);
		waveShader->setMat4("view", view);
		wave->show(waveShader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}