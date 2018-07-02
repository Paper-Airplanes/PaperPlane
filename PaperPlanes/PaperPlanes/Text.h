#pragma once
#include <ft2build.h>

#include FT_FREETYPE_H 

#include "Text.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtx/norm.hpp>

#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <fstream>

#include "Camera.h"
#include <glm/glm/glm.hpp>

#include <map>

#include "Shader.h"



struct Character {

	unsigned int     TextureID;   

	glm::ivec2 Size;      

	glm::ivec2 Bearing;  

	unsigned int     Advance;      

};







class Text {

public:

	Text() {
		init();
	}

	~Text() {
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
	}



	void init() {
		
		//glEnable(GL_CULL_FACE);
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		

		glm::mat4 projection = glm::ortho(0.0f, 1280.0f, 0.0f, 760.0f);

		if (FT_Init_FreeType(&ft))
			std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;


		if (FT_New_Face(ft, "./resources/font/Sweetness Script.ttf", 0, &face))
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

		FT_Set_Pixel_Sizes(face, 0, 48);  

		if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

		for (GLubyte c = 0; c < 128; c++)
		{
	
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}
	
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
	
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			Characters.insert(std::pair<GLchar, Character>(c, character));
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		FT_Done_Face(face);
		FT_Done_FreeType(ft);


		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		//glDisable(GL_CULL_FACE);
	}

	void RenderText(Shader &s, std::string text, float x, float y, float scale, glm::vec3 color) {

		s.use();
		s.setVec3("textColor", color);

		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VAO);


		std::string::const_iterator c;
		for (c = text.begin(); c != text.end(); c++)
		{
			Character ch = Characters[*c];

			GLfloat xpos = x + ch.Bearing.x * scale;
			GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

			GLfloat w = ch.Size.x * scale;
			GLfloat h = ch.Size.y * scale;

			GLfloat vertices[6][4] = {
				{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
			};

			glBindTexture(GL_TEXTURE_2D, ch.TextureID);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLES, 0, 6);

			x += (ch.Advance >> 6) * scale; 
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}



private:

	std::map<char, Character> Characters;

	unsigned int VAO, VBO;

	FT_Library ft;

	FT_Face face;

};
