#pragma once

#include<glad\glad.h>
#include<GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include <iostream>

#include<Camera.h>
#include<Skybox.h>
#include<Shader.h>
#include<Particle.h>
#include<Light.h>


float lastX;
float lastY;
bool firstMouse;
class Game {
private:
	float width, height;
	float deltaTime;
	float lastTime;
	double xpos, ypos;
	//Shader
	Shader* particleShader;
	Shader* skyShader;
	Shader* viewShader;
	//GameObject
	Skybox* skybox;
	Camera* camera;
	Particle* snowparticle;
	GLFWwindow* window;
	Light* light;

public:
	Game(float SCR_WIDTH, float SCR_HEIGHT) {
		//init();
		width = SCR_WIDTH;
		height = SCR_HEIGHT;
		deltaTime = 0.0f;
		lastTime = 0.0f;
		lastX = (float)SCR_WIDTH / 2.0;
		lastY = (float)SCR_HEIGHT / 2.0;

		firstMouse = true;
		//initCamera();
	}
	~Game() {
		delete particleShader;
		delete skyShader;
		delete skybox;
		delete camera;
		
	}
	void initObject() {
		//init shader----------------------------------------------------
		skyShader = new Shader("./shader/skybox.vs", "./shader/skybox.fs");
		particleShader = new Shader("./shader/Particle.vs", "./shader/Particle.fs");
		viewShader = new Shader("./shader/view.vs", "./shader/view.fs");
		//GameObject-----------------------------------------------------
		//init camera
		camera = new Camera(glm::vec3(100.0f, 0.0f, 100.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), 10.0f);
		//init skybox
		vector<std::string> faces{
			"./resources/right.jpg",
			"./resources/left.jpg",
			
			"./resources/top.jpg",
			"./resources/bottom.jpg",
			
			"./resources/front.jpg",
			"./resources/back.jpg",
		};
		//init skybox
		skybox = new Skybox(faces);
		//init particle
		snowparticle = new Particle("./resources/particle.DDS");
		//init light
		//light = new Light(glm::vec3(-3.0f, 3.0f, -1.0f));
		
	}

	void renderView() {
		viewShader->use();
		glm::mat4 projection = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
		viewShader->setMat4("projection", projection);
		glm::mat4 view = camera->GetViewMatrix();
		viewShader->setMat4("view", view);
		viewShader->setMat4("lightSpaceMatrix", light->lightSpaceMatrix);
	}
	void renderSkybox() {
		skyShader->use();
		//skyShader->setInt("skybox", 0);
		glm::mat4 view = glm::mat4(glm::mat3(camera->GetViewMatrix())); // remove translation from the view matrix
		skyShader->setMat4("view", view);
		glm::mat4 projection = glm::perspective(45.0f, width / height, 0.1f, 100.0f);
		skyShader->setMat4("projection", projection);
		skybox->draw();

		
	}
	void renderParticle() {
		particleShader->use();
		snowparticle->prepare(camera);
		snowparticle->draw(camera,particleShader);
	}
	void renderScene() {
		glViewport(0, 0, 1280, 720);
	
		renderParticle();
		renderSkybox();
	}

	void initWindows(float width, float height) {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(width, height, "PaperPlane", NULL, NULL);
		glfwMakeContextCurrent(window);
		glfwSetCursorPos(window, 1280 / 2, 720 / 2);
		//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	}
	void render() {
		initWindows(width, height);
		initObject();
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			glClear(GL_DEPTH_BUFFER_BIT);
			double currentTime = glfwGetTime();
			deltaTime = currentTime - lastTime;
			lastTime = currentTime;

			glfwGetCursorPos(window, &xpos, &ypos);
			mouse_callback(window);
			processInput(window);
			//renderView();
			renderScene();

			glfwSwapBuffers(window);
		
			
		}
		glfwTerminate();
	}

	void processInput(GLFWwindow *window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera->moveForward(deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera->moveBack(deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera->moveLeft(deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera->moveRight(deltaTime);
	}
	//Êó±ê¿ØÖÆÊÓ½Ç----------------------------------------------------
	void mouse_callback(GLFWwindow *window)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.28;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		camera->rotate(xoffset, yoffset);

	}

};



