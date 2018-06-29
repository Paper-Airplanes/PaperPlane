#pragma once

#include<glad\glad.h>
#include<GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include<Skybox.h>

#include <iostream>
#include<Shadow.h>
#include<Model.h>
#include<Camera.h>

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
	
	float rotate_x, rotate_y;
	//Shader
	Shader* particleShader;
	Shader* skyShader;
	Shader* viewShader;
	Shader* shadowShader;
	//GameObject
	Skybox* skybox;
	Camera* camera;
	Model* scenemodel;
	Model* planemodel;
	Particle* snowparticle;
	GLFWwindow* window;
	Light* light;
	Shadow* shadow;

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
		delete viewShader;
		delete shadowShader;
		delete planemodel;
		delete shadow;
		delete skybox;
		delete camera;
		delete scenemodel;
		
	}
	void initObject() {
		//init shader----------------------------------------------------
		skyShader = new Shader("./shader/skybox.vs", "./shader/skybox.fs");
		particleShader = new Shader("./shader/Particle.vs", "./shader/Particle.fs");
		viewShader = new Shader("./shader/1.model_loading.vs", "./shader/1.model_loading.fs");
		shadowShader = new Shader("./shader/shadow_depth.vs", "./shader/shadow_depth.fs");
		viewShader->use();
		viewShader->setInt("shadowMap", 10);

		//GameObject-----------------------------------------------------
		//init camera
		camera = new Camera(glm::vec3(180.0f, 180.0f, -180.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), 50.0f);
		//init skybox
		vector<std::string> faces{
			"./resources/Skybox/right.jpg",
			"./resources/Skybox/left.jpg",
			
			"./resources/Skybox/top.jpg",
			"./resources/Skybox/bottom.jpg",
			
			"./resources/Skybox/front.jpg",
			"./resources/Skybox/back.jpg",
		};
		//init skybox
		//skybox = new Skybox(faces);
		//init particle
		snowparticle = new Particle("./resources/Particle/particle.dds");
		//init model
		scenemodel = new Model("./resources/Scene/Winter.obj");
		//init plane
		planemodel = new Model("./resources/Plane/Plane.obj");
		//init light
		light = new Light(glm::vec3(180.0f, 180.0f, -180.0f));
		//init shadow
		shadow = new Shadow(viewShader);
		
		
	}

	void renderView() {
		viewShader->use();
		glm::mat4 projection = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		viewShader->setMat4("projection", projection);
		glm::mat4 view = camera->GetViewMatrix();
		viewShader->setMat4("view", view);
		viewShader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		viewShader->setVec3("lightPos", light->lightPosition);
		viewShader->setVec3("viewPos", camera->getPosition());
		viewShader->setMat4("lightSpaceMatrix", light->lightSpaceMatrix);
	}
	
	void renderSkybox() {
		skyShader->use();
		skyShader->setInt("skybox", 0);
		glm::mat4 view = glm::mat4(glm::mat3(camera->GetViewMatrix())); // remove translation from the view matrix
		skyShader->setMat4("view", view);
		glm::mat4 projection = glm::perspective(45.0f, width / height, 0.1f, 100.0f);
		skyShader->setMat4("projection", projection);
		skybox->draw();

	}
	
	
	void renderModel() {
		glm::mat4 model;
		
		model = glm::scale(model, glm::vec3(20.02f, 20.02f, 20.02f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		//渲染深度图------------------------------------------------
		shadowShader->use();
		shadowShader->setMat4("lightSpaceMatrix", light->lightSpaceMatrix);
		shadow->draw();
		shadowShader->setMat4("model", model);
		scenemodel->Draw(shadowShader, false);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, 1280, 720);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//渲染物体-------------------------------------------------
		glViewport(0, 0, 1280, 720);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderView();
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_2D, shadow->depthMap);
		
		viewShader->setMat4("model", model);
		scenemodel->Draw(viewShader, true);

		
		
	}

	void renderPlane() {
		

		glm::mat4 model1;
		model1 = glm::translate(model1, camera->getPosition() + 2.0f*glm::normalize(camera->getFront())-0.15f*glm::normalize(camera->getUp()));
		
		model1 = glm::rotate(model1, rotate_y * 180.0f / 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
		model1 = glm::rotate(model1, rotate_x * 180.0f / 3.14f + 5.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		
		model1 = glm::scale(model1, glm::vec3(0.005f, 0.005f, 0.005f));
		viewShader->setMat4("model", model1);
		planemodel->Draw(viewShader, true);
	}

	void renderParticle() {
		particleShader->use();
		snowparticle->prepare(camera);
		snowparticle->draw(camera,particleShader);
	}
	void renderScene() {
		glViewport(0, 0, 1280, 720);
	
		
		
		renderModel();
		renderPlane();
		//renderSkybox();
		renderParticle();
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
		glEnable(GL_DEPTH_TEST);
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			double currentTime = glfwGetTime();
			deltaTime = currentTime - lastTime;
			lastTime = currentTime;

			glfwGetCursorPos(window, &xpos, &ypos);
			mouse_callback(window);
			processInput(window);


			//renderView();
			renderScene();

			glfwSwapBuffers(window);
			//glfwPollEvents();
			
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
	//鼠标控制视角----------------------------------------------------
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

		//计算旋转角
		
		rotate_x = 0.0f, rotate_y = 0.0f;
		glm::vec3 XZ_front = glm::normalize(glm::vec3(camera->getFront().x, 0.0f, camera->getFront().z));
		float x = XZ_front.x, z = XZ_front.z;

		if (x > 0 && z > 0) {
			rotate_y = glm::acos(z);
		}
		else if (x > 0 && z < 0) {
			rotate_y = float(glm::acos(z));
		}
		else if (x < 0 && z > 0) {
			rotate_y = float(-glm::acos(z));
		}
		else if (x < 0 && z < 0) {
			rotate_y = float(6.28 - glm::acos(z));
		}
		glm::vec3 n_front = glm::normalize(camera->getFront());
		x = n_front.x; z = n_front.z; float y = n_front.y;

		if (y < 0.001f && y > -0.001f) rotate_x = 0.0f;
		else rotate_x = -float(glm::asin(y));
		
	}

};



