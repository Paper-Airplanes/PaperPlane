#pragma once
#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

#include<glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include<glm\glm\gtc\matrix_transform.hpp>
#include<glm\glm\gtx\norm.hpp>
#include<glm\glm\glm.hpp>
#include<Camera.h>
#include<Shader.h>

struct particle {
	glm::vec3 pos, speed;
	unsigned char r, g, b, a; // Color
	float size, angle, weight;
	float life; // Remaining life of the particle. if <0 : dead and unused.
	float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

	bool operator<(const particle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}
};

class Particle{
public:
	

	Particle(char* particlePath) {
		init(particlePath);
	}
	~Particle() {
		glDeleteBuffers(1, &colVBO);
		glDeleteBuffers(1, &posVBO);
		glDeleteBuffers(1, &billVBO);
		//glDeleteProgram(programID);
		glDeleteTextures(1, &texture);
		glDeleteVertexArrays(1, &VAO);
		delete[] positionData;
		delete[] colorData;
	}
	void init(char* particlePath) {
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		positionData = new GLfloat[MaxParticles * 4];
		colorData = new GLubyte[MaxParticles * 4];
		for (int i = 0; i<MaxParticles; i++) {
			ParticlesContainer[i].life = -1.0f;
			ParticlesContainer[i].cameradistance = -1.0f;
		}
		texture = loadDDS(particlePath);

		glGenBuffers(1, &billVBO);
		glBindBuffer(GL_ARRAY_BUFFER, billVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer), vertex_buffer, GL_STATIC_DRAW);

		// The VBO containing the positions and sizes of the particles
		glGenBuffers(1, &posVBO);
		glBindBuffer(GL_ARRAY_BUFFER, posVBO);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

		// The VBO containing the colors of the particle
		glGenBuffers(1, &colVBO);
		glBindBuffer(GL_ARRAY_BUFFER, colVBO);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
		//lastTime = glfwGetTime();
		glBindVertexArray(0);
		lastTime = 0;
	}
	void prepare(Camera* camera) {
		
		double currentTime = glfwGetTime();
		double delta = currentTime - lastTime;
		lastTime = currentTime;

		// Generate 10 new particule each millisecond
		int newparticles = (int)(delta*10000.0);
		if (newparticles > (int)(0.016f*10000.0))
			newparticles = (int)(0.016f*10000.0);
		for (int i = 0; i<newparticles; i++) {
			int particleIndex = FindUnusedParticle();
			ParticlesContainer[particleIndex].life = 5.0f; // This particle will live 5 seconds.
			ParticlesContainer[particleIndex].pos = glm::vec3(rand() % 200, 20, rand() % 200);
			float spread = 1.5f;
			glm::vec3 maindir = glm::vec3(0.0f, 0.0f, 0.0f);

			glm::vec3 randomdir = glm::vec3(
				(rand() % 2000 - 1000.0f) / 100.0f,
				//(rand() % 2000 - 1000.0f) / 1000.0f,
				0,
				(rand() % 2000 - 1000.0f) / 100.0f
			);

			ParticlesContainer[particleIndex].speed = glm::vec3(0, 0, 0);

			// Very bad way to generate a random color
			ParticlesContainer[particleIndex].r = 255;// rand() % 256;
			ParticlesContainer[particleIndex].g = 255;// rand() % 256;
			ParticlesContainer[particleIndex].b = 255;// rand() % 256;
			ParticlesContainer[particleIndex].a = (rand() % 256) / 2;

			//ParticlesContainer[particleIndex].size = (rand() % 1000) / 2000.0f + 0.1f;
			ParticlesContainer[particleIndex].size = 0.6f;
		}

		// Simulate all particles
		ParticlesCount = 0;
		for (int i = 0; i < MaxParticles; i++) {
			particle& p = ParticlesContainer[i];

			if (p.life > 0.0f) {

				// Decrease life
				p.life -= delta;
				if (p.life > 0.0f) {
					p.speed += glm::vec3(0.0f, -9.81f, 0.0f) * (float)delta * 0.5f;
					p.pos += p.speed * (float)delta;
					p.cameradistance = glm::length2(p.pos - camera->getPosition());
					//ParticlesContainer[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;

					// Fill the GPU buffer
					positionData[4 * ParticlesCount + 0] = p.pos.x;
					positionData[4 * ParticlesCount + 1] = p.pos.y;
					positionData[4 * ParticlesCount + 2] = p.pos.z;

					positionData[4 * ParticlesCount + 3] = p.size;

					colorData[4 * ParticlesCount + 0] = p.r;
					colorData[4 * ParticlesCount + 1] = p.g;
					colorData[4 * ParticlesCount + 2] = p.b;
					colorData[4 * ParticlesCount + 3] = p.a;

				}
				else {
					// Particles die;
					p.cameradistance = -1.0f;
				}

				ParticlesCount++;

			}
		}

		SortParticles();
	}
	void draw(Camera* camera, Shader* shader) {
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, posVBO);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
		glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, positionData);

		glBindBuffer(GL_ARRAY_BUFFER, colVBO);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
		glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, colorData);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Use our shader
		//glUseProgram(programID);
		shader->use();
		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		shader->setInt("myTextureSampler", 0);
		glm::vec3 right = glm::normalize(glm::cross(camera->getUp(), camera->getFront()));
		shader->setVec3("CameraRight_worldspace", right);
		shader->setVec3("CameraUp_worldspace", camera->getUp());
		glm::mat4 projection = glm::perspective(45.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		shader->setMat4("VP", projection*camera->GetViewMatrix());



		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, billVBO);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : positions of particles' centers
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, posVBO);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : x + y + z + size => 4
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 3rd attribute buffer : particles' colors
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, colVBO);
		glVertexAttribPointer(
			2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : r + g + b + a => 4
			GL_UNSIGNED_BYTE,                 // type
			GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
			0,                                // stride
			(void*)0                          // array buffer offset
		);
		glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
		glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
		glVertexAttribDivisor(2, 1);

		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCount);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glBindVertexArray(0);
	}
private:
	const float vertex_buffer[12] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
		0.5f,  0.5f, 0.0f,
	};
	unsigned int VAO;
	unsigned int billVBO;
	unsigned int posVBO;
	unsigned int colVBO;
	unsigned int texture;

	GLfloat* positionData;
	GLubyte* colorData;
	double lastTime;
	unsigned int ParticlesCount;
	float WIDTH = 1280, HEIGHT = 720;

	static const int MaxParticles = 1000;
	int LastUsedParticle = 0;
	particle ParticlesContainer[MaxParticles];

	int FindUnusedParticle() {

		for (int i = LastUsedParticle; i<MaxParticles; i++) {
			if (ParticlesContainer[i].life < 0) {
				LastUsedParticle = i;
				return i;
			}
		}

		for (int i = 0; i<LastUsedParticle; i++) {
			if (ParticlesContainer[i].life < 0) {
				LastUsedParticle = i;
				return i;
			}
		}

		return 0; // All particles are taken, override the first one
	}

	void SortParticles() {
		std::sort(&ParticlesContainer[0], &ParticlesContainer[MaxParticles]);
	}

	GLuint loadDDS(const char * imagepath) {

		unsigned char header[124];

		FILE *fp;

		/* try to open the file */
		fp = fopen(imagepath, "rb");
		if (fp == NULL) {
			printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar();
			return 0;
		}

		/* verify the type of file */
		char filecode[4];
		fread(filecode, 1, 4, fp);
		if (strncmp(filecode, "DDS ", 4) != 0) {
			fclose(fp);
			return 0;
		}

		/* get the surface desc */
		fread(&header, 124, 1, fp);

		unsigned int height = *(unsigned int*)&(header[8]);
		unsigned int width = *(unsigned int*)&(header[12]);
		unsigned int linearSize = *(unsigned int*)&(header[16]);
		unsigned int mipMapCount = *(unsigned int*)&(header[24]);
		unsigned int fourCC = *(unsigned int*)&(header[80]);


		unsigned char * buffer;
		unsigned int bufsize;
		/* how big is it going to be including all mipmaps? */
		bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
		buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
		fread(buffer, 1, bufsize, fp);
		/* close the file pointer */
		fclose(fp);

		unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
		unsigned int format;
		switch (fourCC)
		{
		case FOURCC_DXT1:
			format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			break;
		case FOURCC_DXT3:
			format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			break;
		case FOURCC_DXT5:
			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			break;
		default:
			free(buffer);
			return 0;
		}

		// Create one OpenGL texture
		GLuint textureID;
		glGenTextures(1, &textureID);

		// "Bind" the newly created texture : all future texture functions will modify this texture
		glBindTexture(GL_TEXTURE_2D, textureID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
		unsigned int offset = 0;

		/* load the mipmaps */
		for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
		{
			unsigned int size = ((width + 3) / 4)*((height + 3) / 4)*blockSize;
			glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
				0, size, buffer + offset);

			offset += size;
			width /= 2;
			height /= 2;

			// Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
			if (width < 1) width = 1;
			if (height < 1) height = 1;

		}

		free(buffer);

		return textureID;


	}
};