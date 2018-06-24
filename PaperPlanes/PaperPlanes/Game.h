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


class Game {
private:
	float width, height;
	//Shader
	Shader* particleShader;
	Shader* skyShader;
	Shader* viewShader;

	//GameObject
	Skybox* skybox;
	Camera* camera;
	GLFWwindow* window;

public:
	Game(float SCR_WIDTH, float SCR_HEIGHT) {
		init();
		width = SCR_WIDTH;
		height = SCR_HEIGHT;
		
		//initCamera();
	}
	~Game() {
		delete particleShader;
		delete &skyShader;
		delete skybox;
		delete camera;
		
	}
	void init() {
		//init shader----------------------------------------------------
		//skyShader = new Shader("./shader/skybox.vs", "./shader/skybox.fs");
		//Shader ourshader("./shader/Particle.vs", "./shader/Particle.fs");
		particleShader = new Shader("./shader/Particle.vs", "./shader/Particle.fs");

		//GameObject-----------------------------------------------------
		//init skybox
		vector<std::string> faces{
			"./resources/right.jpg",
			"./resources/left.jpg",
			"./resources/top.jpg",
			"./resources/bottom.jpg",
			"./resources/back.jpg",
			"./resources/front.jpg"
		};
		skybox = new Skybox(faces);
		//init camera
		camera = new Camera(glm::vec3(-3.0f, 1.0f, -5.0f), glm::vec3(5.0f, -1.0f, 6.0), glm::vec3(0.0f, 1.0f, 0.0f), 2.5f);
	}
	void initCamera() {

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

	}
	void renderScene() {
		glViewport(0, 0, 1280, 720);
		renderSkybox();
		renderParticle();
	}
	/*
	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}
	*/
	void initWindows(float width, float height) {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(width, height, "PaperPlane", NULL, NULL);
		glfwMakeContextCurrent(window);
		//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		glEnable(GL_DEPTH_TEST);
	}
	void render() {
		initWindows(width, height);
		while (!glfwWindowShouldClose(window)) {
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			renderScene();

			glfwSwapBuffers(window);
			glfwPollEvents();
			
		}
		glfwTerminate();
	}

};