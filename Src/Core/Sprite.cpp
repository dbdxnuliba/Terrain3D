//==================================================================================================================|
// Created 2014.05.31 by Daniel L. Watkins
//
// Copyright (C) 2014 Daniel L. Watkins
// This file is licensed under the MIT License.
//==================================================================================================================|

#include "Sprite.h"
#include "Shader.h"

namespace t3d
{
	Sprite::Sprite() :
		mPos(0.0f, 0.0f, 0.0f, 0.0f),
		mScale(1.0f, 1.0f, 1.0f, 1.0f)
	{
		mProgram = 0;
		mTexture = 0;
	}


	Sprite::~Sprite()
	{
		glDeleteProgram(mProgram);
		glDeleteVertexArrays(1, &mVao);
	}


	void Sprite::loadShaders()
	{
		GLuint shaders[2];
		shaders[0] = Shader::loadShader(String(gDefaultPathShaders) + "sprite-vert.glsl", GL_VERTEX_SHADER);
		shaders[1] = Shader::loadShader(String(gDefaultPathShaders) + "sprite-frag.glsl", GL_FRAGMENT_SHADER);

		mProgram = Shader::linkFromShaders(shaders, 2);
	}


	void Sprite::initWithImage(const Image &image)
	{
		mImage = &image;

		glGenTextures(1, &mTexture);
		glBindTexture(GL_TEXTURE_2D, mTexture);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, image.getWidth(), image.getHeight());

		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.getWidth(), image.getHeight(),
						GL_RGBA, GL_UNSIGNED_BYTE, &image.getImageData()[0]);

		glGenVertexArrays(1, &mVao);
		glBindVertexArray(mVao);

		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		
		const float vertexPositions[] =
		{
			0.f, 0.f, 0.0f, 1.f,
			1.f, 0.f, 0.0f, 1.f,
			1.f, 1.f, 0.0f, 1.f,
			
			0.f, 0.f, 0.0f, 1.f,
			1.f, 1.f, 0.0f, 1.f,
			0.f, 1.f, 0.0f, 1.f,
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);

		loadShaders();

		glBindVertexArray(0);
	}


	void Sprite::render(const OpenGLWindow &window) const
	{
		glUseProgram(mProgram);
		glBindVertexArray(mVao);
		
		//apply the offset (pos)
		{
			GLuint loc = glGetUniformLocation(mProgram, "offset");
			glUniform4fv(loc, 1, glm::value_ptr(mPos));
		}

		//scale the texture
		{
			GLuint loc = glGetUniformLocation(mProgram, "scale");
			float sx = (float)mImage->getWidth() / window.getWidth() * 2.0f * mScale.x;
			float sy = (float)mImage->getHeight() / window.getHeight() * 2.0f * mScale.y;

			Vec4f scale(sx, sy, 1.0f, 1.0f);
			glUniform4fv(loc, 1, glm::value_ptr(scale));
		}

		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawArrays(GL_TRIANGLES, 3, 3);

		glBindVertexArray(0);
		glUseProgram(0);
	}


	void Sprite::unloadTexure() const
	{
		glDeleteTextures(1, &mTexture);
	}
};