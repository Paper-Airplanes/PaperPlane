#ifndef SHADER_H
#define SHADER_H

#include <glad\glad.h>
#include <glm\glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

class Shader {
public:
	Shader(const char* vertexPath, const char* fragPath, const char* geometryPath = nullptr) {
		//  ��ɫ��Դ����
		string vertexCode;
		string fragCode;
		string geometryCode;

		//  �����ļ���ȡ��
		ifstream vertexShaderFile;
		ifstream fragShaderFile;
		ifstream geometryShaderFile;

		//  ȷ���ļ���ȡ�����׳��쳣
		vertexShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		fragShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
		geometryShaderFile.exceptions(ifstream::failbit | ifstream::badbit);

		try {
			//  �Ӹ���·�����ļ�
			vertexShaderFile.open(vertexPath);
			fragShaderFile.open(fragPath);
			//  ��ȡ�ļ����嵽����
			stringstream vertexShaderStream, fragShaderStream;
			vertexShaderStream << vertexShaderFile.rdbuf();
			fragShaderStream << fragShaderFile.rdbuf();
			//  �ر��ļ�����
			vertexShaderFile.close();
			fragShaderFile.close();
			//  ��ת��Ϊ�ַ���
			vertexCode = vertexShaderStream.str();
			fragCode = fragShaderStream.str();
			//  �Ƿ�ʹ�ü�����ɫ��
			if (geometryPath != nullptr) {
				geometryShaderFile.open(geometryPath);
				stringstream geometryShaderStream;
				geometryShaderStream << geometryShaderFile.rdbuf();
				geometryShaderFile.close();
				geometryCode = geometryShaderStream.str();
			}
		}
		catch (exception) {
			cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << endl;
		}
		cout << "Test " << endl;
		//  ����ɫ������ת��Ϊc�ַ�����
		const char* vertexShaderCode = vertexCode.c_str();
		const char* fragShaderCode = fragCode.c_str();

		//  ������ɫ��
		unsigned int vertex, fragment;
		int success;
		char infoLog[512];
		//  ������ɫ��
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vertexShaderCode, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");
		//  Ƭ����ɫ��
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fragShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");
		//  ȷ���Ƿ���Ҫ���뼸����ɫ��
		unsigned int geometry;
		if (geometryPath != nullptr) {
			const char* gShaderCode = geometryCode.c_str();
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);
			checkCompileErrors(geometry, "GEOMETRY");
		}
		//  ��ɫ������
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		if (geometryPath != nullptr)
			glAttachShader(ID, geometry);
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");
		//  ������ɫ��
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (geometryPath != nullptr)
			glDeleteShader(geometry);
	}

	//  ʹ����ɫ������
	void use() {
		glUseProgram(ID);
	}


	//  ����������ɫ��Uniform�����ĺ���
	void setBool(const string& name, bool value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	void setInt(const string& name, int value) const {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setFloat(const string& name, float value) const {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setVec2(const string& name, const glm::vec2& value) const {
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec2(const string& name, float x, float y) const {
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}
	void setVec3(const string& name, const glm::vec3& value) const {
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec3(const string& name, float x, float y, float z) const {
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}
	void setVec4(const string& name, glm::vec4& value) const {
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec4(const string& name, float x, float y, float z, float w) const {
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}
	void setMat2(const string& name, const glm::mat2 &mat) const {
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void setMat3(const string& name, const glm::mat3 &mat) const {
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void setMat4(const string& name, const glm::mat4 &mat) const {
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
private:
	unsigned int ID;

	//  �����ɫ��/��ɫ�������Ƿ����ɹ�
	void checkCompileErrors(GLuint shader, string type) {
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM") {
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				cout << "ERROR::SHADER_COMPILATION_ERROR of type."
					<< type << "\n------------------"
					<< infoLog << endl;
			}
		}
		else {
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				cout << "ERROR::PROGRAM_LINKING_ERROR of type: "
					<< type << "\n------------------"
					<< infoLog << endl;
			}
		}
	}
};


#endif // ! SHADER_H
