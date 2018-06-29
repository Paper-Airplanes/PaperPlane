#define GLM_FORCE_RADIANS

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "flag.h"

#include <iostream>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 760;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	Shader flag_shader("FlagShader.vs", "FlagShader.fs");

	int flagW = 40, flagH = 30;
	
	// waveSpeed, waveT, waveAltitude
	SinWave flagwave(3, 7, 3);

	Flag flag(flagW, flagH, flagwave, "fgo.png");

	//flag.testOut();

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glm::mat4 Identity;
		glm::mat4 model = glm::translate(Identity, glm::vec3(-flagW / 2.0f, -flagH / 2.0f, -25.0f));
		glm::mat4 view = glm::translate(Identity, glm::vec3(0.0f, 0.0f, -flagW));
		glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

		glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, flagW);
		glm::vec3 viewPos = glm::vec3(0.0f, 0.0f, flagW);

		flag.Draw(flag_shader, model, view, proj, lightPos, viewPos);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}