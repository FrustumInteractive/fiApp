#include "app.h"


// util
GLuint loadShader(GLenum type, const char* shaderSrc)
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
			FI::LOG("Error compiling shader: ", infoLog);
			free(infoLog);
		}
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}


MyApp::MyApp(const int argc, const char *argv[]) :
#ifdef D3D9
D3D9App(argc, argv)
#else
OGLApp(argc, argv)
#endif
{
	if (!cmdLineArg("-test", false).empty()) {
		FI::LOG("test specified!");
	}

	std::string v = cmdLineArg("-value", true);
	if (!v.empty()) {
		FI::LOG("value: ", v);
	}
}
MyApp::~MyApp()
{

}

void MyApp::onKeyPress(FI::Event e)
{
	eKeyCode kc = (eKeyCode)e.data_uint32()[0];
	switch(kc)
	{
		case KEY_ESC:
		case KEY_Q:
			m_bQuit = true;
			break;
		default:
			return;
	}
}

void MyApp::onMouseLeftClick(FI::Event e)
{
	float x = e.data_float()[0];
	float y = e.data_float()[1];
	FI::LOG("LMB click (", x, ",", y, ")");
}

void MyApp::onMouseRightClick(FI::Event e)
{
	float x = e.data_float()[0];
	float y = e.data_float()[1];
	FI::LOG("RMB click (", x, ",", y, ")");
}

void MyApp::initScene()
{
	FI::LOG("initializing scene...");

	// GL 2.0
	const char vShaderStrGL2[] =
	"#version 120				\n"
	"attribute vec4 vPosition;  \n"
	"void main()                \n"
	"{                          \n"
	"  gl_Position = vPosition; \n"
	"}                          \n";

	const char fShaderStrGL2[] =
	"#version 120								\n"
	"void main()                                \n"
	"{                                          \n"
	"  gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); \n"
	"}											\n";

	//GL ES 2
	const char vShaderStrGLES2[] =
	"precision mediump float;   \n"
	"attribute vec4 vPosition;  \n"
	"void main()                \n"
	"{                          \n"
	"  gl_Position = vPosition; \n"
	"}                          \n";

	const char fShaderStrGLES2[] =
	"precision mediump float;                   \n"
	"void main()                                \n"
	"{                                          \n"
	"  gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); \n"
	"}                                          \n";


	// OpenGL 2.0
	const char vShaderStrGL3[] =
	"#version 150				\n"
	"in vec4 vPosition;  		\n"
	"out vec4 position;			\n"
	"void main()                \n"
	"{                          \n"
	"  position = vPosition; 	\n"
	"}                          \n";

	const char fShaderStrGL3[] =
	"#version 150								\n"
	"out vec4 fragColor;						\n"
	"void main()                                \n"
	"{                                          \n"
	"  fragColor = vec4(1.0, 0.0, 0.0, 1.0); 	\n"
	"}                                          \n";

	GLuint vshad = 0;
	GLuint fshad = 0;

	if(majorVersion() >= 3)
	{
		vshad = loadShader(GL_VERTEX_SHADER, vShaderStrGL3);
		fshad = loadShader(GL_FRAGMENT_SHADER, fShaderStrGL3);
	}
	else if(majorVersion() > 2)
	{
		vshad = loadShader(GL_VERTEX_SHADER, vShaderStrGLES2);
		fshad = loadShader(GL_FRAGMENT_SHADER, fShaderStrGLES2);
	}
	else
	{
		vshad = loadShader(GL_VERTEX_SHADER, vShaderStrGL2);
		fshad = loadShader(GL_FRAGMENT_SHADER, fShaderStrGL2);
	}
	
	// Create the program object
	m_shaderProgram = glCreateProgram();
	if(m_shaderProgram == 0)
	{
		FI::LOG("failed to create shader program.");
		return;
	}

	glAttachShader(m_shaderProgram, vshad);
	glAttachShader(m_shaderProgram, fshad);
	
	// Bind vPosition to attribute 0
	glBindAttribLocation(m_shaderProgram, 0, "vPosition");
	
	// Link the program
	glLinkProgram(m_shaderProgram);

	// Check the link status
	GLint linked;
	glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &linked);

	if(!linked)
	{
		GLint infoLen = 0;
		glGetProgramiv(m_shaderProgram, GL_INFO_LOG_LENGTH, &infoLen);
		if(infoLen > 1)
		{
			char* infoLog = (char*)malloc(sizeof(char) * infoLen);
			glGetProgramInfoLog(m_shaderProgram, infoLen, NULL, infoLog);
			FI::LOG("Error linking program: ", infoLog);
			free(infoLog);
		}
		glDeleteProgram(m_shaderProgram);
		return;
	}
	
	// Use the program object
	glUseProgram(m_shaderProgram);

	m_vPosAttrib = glGetAttribLocation(m_shaderProgram, "vPosition");
	
	GLfloat vVertices[] = {
		0.0f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f
	};

	// Load the vertex data
	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);

	// Set the viewport
	glViewport(0, 0, m_width, m_height);
	
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glClearColor(0.0f, 0.2f, 0.1f, 1.0f);
}

void MyApp::deinitScene()
{
	FI::LOG("de-initializing scene");
}

void MyApp::drawScene()
{
#ifndef D3D

	glClear(GL_COLOR_BUFFER_BIT);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glEnableVertexAttribArray(0); // we bound 0 to vPosition earlier
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 3);

#else
	// draw D3D9 stuff
#endif /* D3D */
}
