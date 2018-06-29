//  ����
//  waveWidth
//	waveHeight��ˮ����ߣ�0-1
//  model
//  view
//  projection����
//  lightPos ��Դλ��
//  viewPos �ӽ�
//  texturePath ��ͼλ��

//  ʹ��wave����Ҫ����ʵ��·���޸���ɫ���ļ�·��

#ifndef WAVE_H
#define WAVE_H

#include <glad\glad.h>
#include <glfw\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <string>

#include "Shader.h"
#include "TextureProcess.h"

using namespace std;

#define PI 3.14159265358979323846
#define GRID_HEIGHT 30
#define GRID_WIDTH 30

//  ��ɫ��·��������ʵ��·���޸�
string gridVertexShader = "D:/Projects/VS-Projects/Computer-Graphics-02/Final-Project/src/shader/gridVertexShader_2.vs";
string gridFragShader = "D:/Projects/VS-Projects/Computer-Graphics-02/Final-Project/src/shader/gridFragShader.fs";

class Wave {
public:
	Wave(float waveWidth, float waveHeight, 
		glm::mat4 model, glm::mat4 view, glm::mat4 projection, 
		glm::vec3 _lightPos, glm::vec3 _viewPos,
		string texturePath);

	~Wave();

	void show() {
		time = glfwGetTime();
		waveProgram->use();
		waveProgram->setFloat("curTime", time);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(gridVAO);
		glDrawArrays(GL_TRIANGLES, 0, gridWidth * gridHeight * 6);
	}

private:
	//  ˮ����ȸ߶ȣ�0-1
	float waveWidth = 0.5;
	float waveHeight = 0.5;
	//  ����
	glm::mat4 identity;
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
	//  ��Դ���ӽ�λ��
	glm::vec3 lightPos;
	glm::vec3 viewPos;
	//  �����߸���
	int gridWidth = GRID_WIDTH+1;
	int gridHeight = GRID_HEIGHT+1;
	//  ˮ������ˮ���������Ǿ���
	//  waveGrid[GRID_WIDTH + 1][GRID_HEIGHT + 1][2]
	float waveGrid[GRID_WIDTH + 1][GRID_HEIGHT + 1][2] = { 0 };
	//  waveGridTriVertices[GRID_HEIGHT * GRID_WIDTH * 6][5]
	float waveGridTriVertices[GRID_HEIGHT * GRID_WIDTH * 6][5] = { 0 };

	//  ʱ��
	float time = 0.0f;

	//  VAO & VBO
	unsigned int gridVAO = 0;
	unsigned int gridVBO = 0;

	//  ��ɫ������
	Shader* waveProgram = NULL;

	//  ��ͼ
	string texturePath = "";
	unsigned int texture = 0;

	//  ��ʼ�����񶥵�
	void initGrid();

	//  ���������ζ������
	void genGridTriVertices();

	//  ����VBO, VAO
	void createVBO_VAO(string type);

	//  ����Uniform����
	void setUniform();

	//  ������ͼ
	void setTexture();
};


//  ���캯��
Wave::Wave(float _waveWidth, float _waveHeight, 
		glm::mat4 _model, glm::mat4 _view, glm::mat4 _projection, 
		glm::vec3 _lightPos, glm::vec3 _viewPos,
		string _texturePath) {
	waveWidth = _waveWidth;
	waveHeight = _waveHeight;
	model = _model;
	view = _view;
	projection = _projection;
	lightPos = _lightPos;
	viewPos = _viewPos;
	texturePath = _texturePath;

	//allocateSpace();

	initGrid();
	genGridTriVertices();

	createVBO_VAO("grid");

	waveProgram = new Shader(gridVertexShader.c_str(), gridFragShader.c_str(), nullptr);
	setUniform();
	setTexture();
}

//  ��������
Wave::~Wave() {
}

//  ��ʼ�����񶥵�
void Wave::initGrid() {
	float xStride = waveWidth / (gridWidth - 1),
		yStride = waveHeight / (gridHeight - 1);
	for (int i = 0; i < gridWidth; i++) {
		for (int j = 0; j < gridHeight; j++) {
			waveGrid[i][j][0] += (xStride * i);
			waveGrid[i][j][1] += (yStride * j);
		}
	}
}


//  �������������ζ������ array[length][5]��λ��[3]+��ͼ����[2]
void Wave::genGridTriVertices() {
	int WIDTH = gridWidth,
		HEIGHT = gridHeight,
		DIM_2 = 2;
	int LENGTH = gridWidth * gridHeight * 6,
		DIM_5 = 5;

	//cout << "WIDTH" << WIDTH << endl;
	//cout << "HEIGHT" << HEIGHT << endl;
	//cout << "LENGTH" << LENGTH << endl;

	float waveFactor = 30;
	float zStride = 3 * PI / WIDTH;

	float xTextureStride = 1.0 / (WIDTH - 1), yTextureStride = 1.0 / (HEIGHT - 1);

	if (DIM_2 != 2 || DIM_5 != 5) {
		//cout << "DIMENTION WRONG" << endl;
		return;
	}

	//  ÿ�α����������еĶ���
	float*** twoLinePoints = new float**[WIDTH];
	for (int i = 0; i < WIDTH; i++) {
		twoLinePoints[i] = new float*[HEIGHT];
		for (int j = 0; j < HEIGHT; j++) {
			twoLinePoints[i][j] = new float[2];
			for (int k = 0; k < 2; k++) {
				twoLinePoints[i][j][k] = 0;
			}
		}
	}

	int line = 0, offset = 0;
	int ptr = 0;

	for (int i = 0; i < HEIGHT - 1; i++) {
		//  ȡ���еĵ�
		for (int p = 0; p < 2 * WIDTH; p++) {
			line = p / WIDTH;
			offset = p % WIDTH;
			twoLinePoints[offset][line][0] = waveGrid[offset][i + line][0];
			twoLinePoints[offset][line][1] = waveGrid[offset][i + line][1];
		}
		//  ��2*WIDTH����(x, y)ת��Ϊ 3*2*(WIDTH-1)��(x, y, z)
		for (int m = 0; m < 2; m++) {
			for (int n = 0; n < WIDTH - 1; n++) {
				//	��ȡ (width - 1)*2 �������εĶ���
				//cout << "ptr\t" << ptr << endl;
				if (m == 0) {
					waveGridTriVertices[ptr][0] = twoLinePoints[n][m][0];
					waveGridTriVertices[ptr][1] = twoLinePoints[n][m][1];
					waveGridTriVertices[ptr][2] = 0;
					//waveGridTriVertices[ptr][2] = sin(zStride * n) / waveFactor;
					waveGridTriVertices[ptr][3] = xTextureStride * n;
					waveGridTriVertices[ptr][4] = yTextureStride * i;
					ptr++;
					waveGridTriVertices[ptr][0] = twoLinePoints[n + 1][m][0];
					waveGridTriVertices[ptr][1] = twoLinePoints[n + 1][m][1];
					waveGridTriVertices[ptr][2] = 0;
					//waveGridTriVertices[ptr][2] = sin(zStride * (n + 1)) / waveFactor;
					waveGridTriVertices[ptr][3] = xTextureStride * (n + 1);
					waveGridTriVertices[ptr][4] = yTextureStride * i;
					ptr++;
					waveGridTriVertices[ptr][0] = twoLinePoints[n + 1][m + 1][0];
					waveGridTriVertices[ptr][1] = twoLinePoints[n + 1][m + 1][1];
					waveGridTriVertices[ptr][2] = 0;
					//waveGridTriVertices[ptr][2] = sin(zStride * (n + 1)) / waveFactor;
					waveGridTriVertices[ptr][3] = xTextureStride * (n + 1);
					waveGridTriVertices[ptr][4] = yTextureStride * (i + 1);
					ptr++;
				}
				else {
					waveGridTriVertices[ptr][0] = twoLinePoints[n][m][0];
					waveGridTriVertices[ptr][1] = twoLinePoints[n][m][1];
					waveGridTriVertices[ptr][2] = 0;
					//waveGridTriVertices[ptr][2] = sin(zStride * n) / waveFactor;
					waveGridTriVertices[ptr][3] = xTextureStride * n;
					waveGridTriVertices[ptr][4] = yTextureStride * (i + 1);
					ptr++;
					waveGridTriVertices[ptr][0] = twoLinePoints[n][m - 1][0];
					waveGridTriVertices[ptr][1] = twoLinePoints[n][m - 1][1];
					waveGridTriVertices[ptr][2] = 0;
					//waveGridTriVertices[ptr][2] = sin(zStride * n) / waveFactor;
					waveGridTriVertices[ptr][3] = xTextureStride * n;
					waveGridTriVertices[ptr][4] = yTextureStride * i;
					ptr++;
					waveGridTriVertices[ptr][0] = twoLinePoints[n + 1][m][0];
					waveGridTriVertices[ptr][1] = twoLinePoints[n + 1][m][1];
					waveGridTriVertices[ptr][2] = 0;
					//waveGridTriVertices[ptr][2] = sin(zStride * (n+1)) / waveFactor;
					waveGridTriVertices[ptr][3] = xTextureStride * (n + 1);
					waveGridTriVertices[ptr][4] = yTextureStride * (i + 1);
					ptr++;
				}
			}
		}
	}
}


//  ����VAO��VBO
void Wave::createVBO_VAO(string type) {

	glGenVertexArrays(1, &(this->gridVAO));
	glBindVertexArray(this->gridVAO);
	glGenBuffers(1, &gridVBO);
	glBindBuffer(GL_ARRAY_BUFFER, gridVBO);

	if (type == "grid") {
		//  ���ƶ������ݵ������ڴ��У����Ӷ�������
		glBufferData(GL_ARRAY_BUFFER, sizeof(waveGridTriVertices), waveGridTriVertices, GL_STATIC_DRAW);
		//  λ������
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)0);
		glEnableVertexAttribArray(0);
		//  ��ͼ��������
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(1);
	}
	else {
		cout << "OBJ TYPE ERROR" << endl;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Wave::setUniform() {
	waveProgram->use();
	waveProgram->setMat4("model", model);
	waveProgram->setMat4("view", view);
	waveProgram->setMat4("projection", projection);
	waveProgram->setFloat("curTime", time);
	waveProgram->setVec3("lightPos", lightPos);
	waveProgram->setVec3("viewPos", viewPos);
}

//  ������ͼ
void Wave::setTexture() {
	stbi_set_flip_vertically_on_load(true);

	int texWidth, texHeight, texChannels;
	unsigned char* texData = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, 0);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (texData) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		cout << "Failed to load texture" << endl;
	}
	stbi_image_free(texData);
}


#endif // !WAVE_H