#pragma once
#include <glad\glad.h>
#include <GLFW\glfw3.h>

class Shadow {
public:
	unsigned int depthMapFBO;
	unsigned int depthMap;
	Shadow(Shader* shader) {
		init(shader);
	}
	void draw() {
		glViewport(0, 0, 8192, 8192);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	unsigned int GetDepthMap() {
		return depthMap;
	}

private:
	void init(Shader* shader) {
		//glGenFramebuffers(1, &depthMapFBO);
		glGenTextures(1, &depthMap);
		//glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 8192, 8192, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		/*
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		*/
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		
		// 使用深度纹理作为缓冲
		glGenFramebuffers(1, &depthMapFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		/*
		shader->use();
		shader->setInt("shadowMap", 0);
		*/
	}
};