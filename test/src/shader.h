#ifndef _SHADER_H
#define _SHADER_H

#include "app.h"

GLuint compileShader(GLenum type, const char* shaderSrc)
{
	GLuint shader;
	GLint compiled;

	// Create the shader object
	shader = glCreateShader(type);

	if(shader == 0)
	return 0;

	glShaderSource(shader, 1, &shaderSrc, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

	if(!compiled)
	{
		GLint infoLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
		if(infoLen > 1)
		{
			char* infoLog = (char*)malloc(sizeof(char) * infoLen);
			glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
			std::string typestr;
			switch (type)
			{
				case GL_VERTEX_SHADER:
					typestr = "vertex"; break;
				case GL_FRAGMENT_SHADER:
					typestr = "fragment"; break;
			}
			std::string str("Error compiling " + typestr + " shader: ");
			FI::LOG(str, infoLog);
			free(infoLog);
		}
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

GLuint buildShaderProgram(GLuint vs, GLuint fs)
{
	// Create the program object
	auto shaderProgram = glCreateProgram();
	if (shaderProgram == 0)
	{
		FI::LOG("failed to create shader program.");
		return 0;
	}

	glAttachShader(shaderProgram, vs);
	glAttachShader(shaderProgram, fs);
	
	// Bind vPosition to attribute 0, vColor to 1
	glBindAttribLocation(shaderProgram, 0, "vPosition");
	glBindAttribLocation(shaderProgram, 1, "vColor");
	
	// Link the program
	glLinkProgram(shaderProgram);

	// Check the link status
	GLint linked;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);

	if (!linked)
	{
		GLint infoLen = 0;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1)
		{
			char* infoLog = (char*)malloc(sizeof(char) * infoLen);
			glGetProgramInfoLog(shaderProgram, infoLen, NULL, infoLog);
			FI::LOG("Error linking program: ", infoLog);
			free(infoLog);
		}
		glDeleteProgram(shaderProgram);
		return 0;
	}

	return shaderProgram;
}


// GL 2.0
const char vShaderStrGL2[] = R"VS(
	#version 120
	attribute vec3 vPosition;
	attribute vec3 vColor;
	varying vec4 color;
	void main()
	{
		color = vec4(vColor, 1.0);
		gl_Position = vec4(vPosition, 1.0);
	}
)VS";

const char fShaderStrGL2[] = R"FS(
	#version 120
	varying vec4 color;
	void main()
	{
		gl_FragColor = color;
	}
)FS";


// GL ES 2
const char vShaderStrGLES2[] = R"VS(
	#version 100
	precision mediump float;
	attribute vec3 vPosition;
	attribute vec3 vColor;
	varying vec4 color;
	void main()
	{
		color = vec4(vColor, 1.0);
		gl_Position = vec4(vPosition, 1.0);
	}
)VS";

const char fShaderStrGLES2[] = R"FS(
	#version 100
	precision mediump float;
	varying vec4 color;
	void main()
	{
		gl_FragColor = color;
	}
)FS";


// GL 3.0
const char vShaderStrGL3[] = R"VS(
	#version 150
	in vec3 vPosition;
	in vec3 vColor;
	out vec4 color;
	void main()
	{
		color = vec4(vColor, 1.0);
		gl_Position = vec4(vPosition, 1.0);
	}
)VS";

const char fShaderStrGL3[] = R"FS(
	#version 150
	in vec4 color;
	out vec4 fragColor;
	void main()
	{
		fragColor = color;
	}
)FS";

#endif // _SHADER_H