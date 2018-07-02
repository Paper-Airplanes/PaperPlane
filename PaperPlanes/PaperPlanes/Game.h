#pragma once
#include<glad\glad.h>
#include<GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtx/norm.hpp>
#include<Skybox.h>
#include<Wave.h>
#include <iostream>
#include<Shadow.h>
#include<Model.h>
#include<Camera.h>
#include<MyWindow.h>
#include<Shader.h>
#include<Particle.h>
#include<Light.h>
#include <Collision.h>
#include<Text.h>

float lastX;
float lastY;
bool firstMouse;
class Game {
private:
	float width, height;
	float deltaTime;
	float lastTime;
	double xpos, ypos;
	int season;
	float rotate_x, rotate_y;
	Collision collision;
	//Shader
	Shader* particleShader;
	Shader* skyShader;
	Shader* viewShader;
	Shader* shadowShader;
	Shader* waveShader;
	Shader* lakeShader;
	Shader* textShader;
	//GameObject
	Skybox* skybox;
	Camera* camera;
	Model* wintermodel;
	Model* springmodel;
	Model* summermodel;
	Model* autumnmodel;
	Model* planemodel;
	Particle* snowparticle;
	GLFWwindow* window;
	Light* light;
	Shadow* shadow;
	Wave* wave;
	Wave* lake;
	Model* winter;
	Model* spring;
	Model* summer;
	Model* autumn;
	Text* text;

public:
	Game(float SCR_WIDTH, float SCR_HEIGHT) {
		//init();
		width = SCR_WIDTH;
		height = SCR_HEIGHT;
		deltaTime = 0.0f;
		lastTime = 0.0f;
		lastX = (float)SCR_WIDTH / 2.0;
		lastY = (float)SCR_HEIGHT / 2.0;
		season = 0;
		firstMouse = true;
		//initCamera();
	}
	~Game() {
		delete particleShader;
		delete skyShader;
		delete viewShader;
		delete shadowShader;
		delete waveShader;
		delete planemodel;
		delete shadow;
		delete skybox;
		delete camera;
		delete wintermodel;
		delete summermodel;
		delete springmodel;
		delete autumnmodel;
		delete wave;
		delete lakeShader;
		delete winter;
	}
	void initObject() {
		//init shader----------------------------------------------------
		skyShader = new Shader("./shader/skybox.vs", "./shader/skybox.fs");
		particleShader = new Shader("./shader/Particle.vs", "./shader/Particle.fs");
		viewShader = new Shader("./shader/1.model_loading.vs", "./shader/1.model_loading.fs");
		shadowShader = new Shader("./shader/shadow_depth.vs", "./shader/shadow_depth.fs");
		waveShader = new Shader("./shader/gridVertexShader.vs", "./shader/gridFragShader.fs");
		lakeShader = new Shader("./shader/gridVertexShader1.vs", "./shader/gridFragShader.fs");
		//textShader = new Shader("./shader/textshader.vs", "./shader/textshader.fs");
		viewShader->use();
		viewShader->setInt("shadowMap", 10);
		
		//GameObject-----------------------------------------------------
		//init camera
		camera = new Camera(glm::vec3(50.0f, 50.0f, 50.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), 50.0f);
		//init skybox
		/*
		vector<std::string> faces{
			"./resources/Skybox/right.jpg",
			"./resources/Skybox/left.jpg",
			
			"./resources/Skybox/top.jpg",
			"./resources/Skybox/bottom.jpg",
			
			"./resources/Skybox/front.jpg",
			"./resources/Skybox/back.jpg",
		};
		*/
		vector<std::string> faces2{
			"./resources/Skybox/starfield_rt.tga",
			"./resources/Skybox/starfield_lf.tga",
			"./resources/Skybox/starfield_up.tga",
			"./resources/Skybox/starfield_dn.tga",
			"./resources/Skybox/starfield_bk.tga",
			"./resources/Skybox/starfield_ft.tga",
			
		};
		//init skybox
		skybox = new Skybox(faces2);
		//init particle
		snowparticle = new Particle("./resources/Particle/particle.dds");
		//init model
		wintermodel = new Model("./resources/Scene/Winter.obj");
		springmodel = new Model("./resources/Scene/Spring.obj");
		summermodel = new Model("./resources/Scene/Summer.obj");
		autumnmodel = new Model("./resources/Scene/Autumn.obj");
		//init plane
		planemodel = new Model("./resources/Plane/Plane.obj");
		//init seasonlogo
		winter = new Model("./resources/SeasonLogo/SpringLogo.obj");
		spring = new Model("./resources/SeasonLogo/SummerLogo.obj");
		summer = new Model("./resources/SeasonLogo/AutumnLogo.obj");
		autumn = new Model("./resources/SeasonLogo/WinterLogo.obj");
		//init light
		light = new Light(glm::vec3(180.0f, 180.0f, -180.0f));
		//light = new Light(glm::vec3(0.25, 0.25f, 1.0f));
		//init shadow
		shadow = new Shadow(viewShader);
		//init wave
		wave = new Wave(0.8, 1.2, "./resources/texture/water-texture-2.tga");
		//init lake
		lake = new Wave(0.6, 0.6, "./resources/texture/water-texture-2.tga");
		//init text
		//text = new Text();
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

	void renderText() {
		glm::vec3 color = glm::vec3(0.4f, 0.2f, 0.8f);
		glm::mat4 projection = glm::ortho(0.0f, 1280.0f, 0.0f, 760.0f);
		textShader->use();
		textShader->setMat4("projection", projection);
		text->RenderText(*textShader, "Spring", 15.0f, 15.0f, 1.5f, glm::vec3(0.9f, 0.9f, 0.9f));
	}
	
	
	void renderModel(Model* seasonmode) {
		Model* drawModel = seasonmode;
		glm::mat4 model;
		
		model = glm::scale(model, glm::vec3(20.02f, 20.02f, 20.02f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		//渲染深度图------------------------------------------------
		shadowShader->use();
		shadowShader->setMat4("lightSpaceMatrix", light->lightSpaceMatrix);
		shadow->draw();
		shadowShader->setMat4("model", model);
		drawModel->Draw(shadowShader, false);
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
		drawModel->Draw(viewShader, true);

		
		
	}

	void renderLogo(Model* logo) {
		//renderView();
		glm::mat4 model;

		//model = glm::scale(model, glm::vec3(20.02f, 20.02f, 20.02f));
		
		model = glm::translate(model, glm::vec3(100.0f, 100.0f, 100.0f));
		model = glm::scale(model, glm::vec3(5.02f, 5.02f, 5.02f));
		//model = glm::rotate(model, 90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		//model = glm::rotate(model, -90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, (float)glfwGetTime()*45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		
		viewShader->setMat4("model", model);
		logo->Draw(viewShader, true);
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
	void renderWave() {
		waveShader->use();
		waveShader->setVec3("lightPos", light->lightPosition);
		glm::mat4 model;
		model = glm::scale(model, glm::vec3(300.02f, 300.02f, 300.02f));
		model = glm::rotate(model, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-0.15f, -0.33f, 0.015f));
		waveShader->setMat4("model", model);
		glm::mat4 projection = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		waveShader->setMat4("projection", projection);
		waveShader->setVec3("viewPos", camera->getPosition());
		waveShader->setMat4("view", camera->GetViewMatrix());
		wave->show(waveShader);
	}
	void renderLake() {
		lakeShader->use();
		lakeShader->setVec3("lightPos", light->lightPosition);
		glm::mat4 model;
		model = glm::scale(model, glm::vec3(200.02f, 200.02f, 200.02f));
		model = glm::rotate(model, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.85f, 0.1f, -0.0035f));
		lakeShader->setMat4("model", model);
		glm::mat4 projection = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		lakeShader->setMat4("projection", projection);
		lakeShader->setVec3("viewPos", camera->getPosition());
		lakeShader->setMat4("view", camera->GetViewMatrix());
		lake->show(lakeShader);
	}

	void renderParticle() {
		particleShader->use();
		snowparticle->prepare(camera);
		snowparticle->draw(camera,particleShader);
	}
	void renderScene() {
		glViewport(0, 0, 1280, 720);
	
		
		if (season % 4 == 0) {
			renderModel(wintermodel);
		}
		else if (season % 4 == 1) {
			renderModel(springmodel);
		}
		else if (season % 4 == 2) {
			renderModel(summermodel);
		}
		else if (season % 4 == 3) {
			renderModel(autumnmodel);
		}
		//
		if (season % 4 == 0) {
			renderLogo(winter);
		}
		else if (season % 4 == 1) {
			renderLogo(spring);
		}
		else if (season % 4 == 2) {
			renderLogo(summer);
		}
		else if (season % 4 == 3) {
			renderLogo(autumn);
		}
		//renderLogo();
		renderPlane();
		renderWave();
		renderLake();
		renderSkybox();
		//renderText();
		if (season % 4 == 0) {
			renderParticle();
		}
	}

	void initWindows(float width, float height) {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(width, height, "PaperPlane", NULL, NULL);
		glfwMakeContextCurrent(window);
		glfwSetCursorPos(window, 1280 / 2, 720 / 2);
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		//glEnable(GL_DEPTH_TEST);
		//glDepthFunc(GL_LESS);
	}
	void render() {
		//initWindows(width, height);
		if (!myCreateWindow(window, "PaperPlane", 1280, 760)) {
			//return -1;
		}
		
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		initObject();

		
		
		//glClearColor(0.3,0.3,0.3,1.0);
		int flag = 0;
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
			double currentTime = glfwGetTime();
			deltaTime = currentTime - lastTime;
			lastTime = currentTime;

			glfwGetCursorPos(window, &xpos, &ypos);
			mouse_callback(window);
			processInput(window);

			renderView();
			renderScene();
			collision.updateCameraBody(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
			if (collision.testCollision()) {
				//cout << "fuck" << endl;
				if (flag == 0) {
					flag = 1;
					season++;
				}
			}
			else {
				flag = 0;
			}
			glfwSwapBuffers(window);
			glfwPollEvents();
			
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



