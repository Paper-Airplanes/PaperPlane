#pragma once

#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

struct Vertex {
	glm::vec3 Position;
	glm::vec2 TexCoords;
};

struct Texture {
	unsigned int id;
};

class SinWave {
public:
	float Speed;
	float T;
	float Altitude;

	SinWave() {
		Speed = T = Altitude = 0;
	}
	SinWave(float Speed, float Tint, float Altitude) {
		this->Speed = Speed;
		this->T = Tint * 3.14159f;
		this->Altitude = Altitude;
	}
};

class Flag {

public:
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	Texture face;
	SinWave FlagWave;

	Flag(int FlagWidth, int FlagHeight, SinWave FlagWave, string FlagFaceName) {
		// init wave
		this->FlagWave = FlagWave;

		// init vertex
		float WidthUnit = 1.0f / FlagWidth;
		float HeightUnit = 1.0f / FlagHeight;
		Vertex tmp;
		for (int i = 0; i <= FlagWidth; i++) {
			for (int j = 0; j <= FlagHeight; j++) {
				tmp.Position = glm::vec3(i, j, 0);
				tmp.TexCoords = glm::vec2(i * WidthUnit, j * HeightUnit);

				vertices.push_back(tmp);
			}
		}

		// init index
		for (int i = 0; i < FlagWidth; i++) {
			for (int j = 0; j < FlagHeight; j++) {
				// left_bottom triangle
				indices.push_back(i * (FlagHeight + 1) + j);
				indices.push_back(i * (FlagHeight + 1) + (j + 1));
				indices.push_back((i + 1) * (FlagHeight + 1) + j);

				// right_top triangle
				indices.push_back(i * (FlagHeight + 1) + (j + 1));
				indices.push_back((i + 1) * (FlagHeight + 1) + (j + 1));
				indices.push_back((i + 1) * (FlagHeight + 1) + j);
			}
		}

		// init face texture
		stbi_set_flip_vertically_on_load(true);

		int faceW, faceH, faceChannel;
		unsigned char *data = stbi_load(FlagFaceName.c_str(), &faceW, &faceH, &faceChannel, 0);

		glGenTextures(1, &face.id);
		glBindTexture(GL_TEXTURE_2D, face.id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (!data) {
			cout << "Failed to read image as texture!" << endl;
		}

		cout << faceChannel << endl;

		if (faceChannel == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, faceW, faceH, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		} else if (faceChannel == 4) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, faceW, faceH, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		
		stbi_image_free(data);

		// bind to buffers
		BindBuffers();
	}
	void Draw(Shader shader, glm::mat4 model, glm::mat4 view, glm::mat4 proj, 
		        glm::vec3 lightPos, glm::vec3 viewPos) {
		shader.use();
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("proj", proj);
		shader.setVec3("lightPos", lightPos);
		shader.setVec3("viewPos", viewPos);

		//cout << FlagWave.Speed  << " " << FlagWave.T << " " << FlagWave.Altitude <<endl;

		float time = glfwGetTime();
		shader.setFloat("time", time);
		shader.setFloat("waveSpeed", FlagWave.Speed);
		shader.setFloat("waveT", FlagWave.T);
		shader.setFloat("waveAltitude", FlagWave.Altitude);

		glBindTexture(GL_TEXTURE_2D, face.id);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void testOut() {
		cout << "vertices size is" << vertices.size() << endl;
		for (int i = 0; i < vertices.size(); i++) {
			cout << vertices[i].Position.x << " ";
			cout << vertices[i].Position.y << " ";
			cout << vertices[i].Position.z << " ";
			cout << endl;

			cout << vertices[i].TexCoords.x << " ";
			cout << vertices[i].TexCoords.y << " ";
			cout << endl;
		}

		cout << "indices size is" << indices.size() << endl;
		for (int i = 0; i < indices.size(); i++) {
			cout << indices[i] << " ";
		}
	}


private:
	unsigned int VAO, VBO, EBO;
	void BindBuffers() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0].Position.x, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0);
	}
};
#endif