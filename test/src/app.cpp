#include "app.h"
#include "shader.h"
#include <string>

// - Util -
void checkError(const char* str, const char* str2 = nullptr, bool consume = false)
{
	int e = glGetError();

	if (consume) return;

	std::string estr;
	switch (e)
	{
		case GL_INVALID_VALUE:
			estr = "GL_INVALID_VALUE"; break;
		case GL_INVALID_ENUM:
			estr = "GL_INVALID_ENUM"; break;
		case GL_INVALID_OPERATION:
			estr = "GL_INVALID_OPERATION"; break;
		default:
			estr = std::to_string(e);
	}

	if (e)
	{
		FI::LOG("GL error <", str2, ">: ", estr, " in: ", str );
	}
}
void consumeError()
{
	checkError(0, 0, true);
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

		case KEY_W: FI::LOG("Key W pressed"); break;
		case KEY_A: FI::LOG("Key A pressed"); break;
		case KEY_S: FI::LOG("Key S pressed"); break;
		case KEY_D: FI::LOG("Key D pressed"); break;

		default:
			return;
	}
}

void MyApp::onKeyRelease(FI::Event e)
{
	eKeyCode kc = (eKeyCode)e.data_uint32()[0];
	switch(kc)
	{
		case KEY_ESC:
		case KEY_Q:
			m_bQuit = true;
			break;

		case KEY_W: FI::LOG("Key W released"); break;
		case KEY_A: FI::LOG("Key A released"); break;
		case KEY_S: FI::LOG("Key S released"); break;
		case KEY_D: FI::LOG("Key D released"); break;

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

void MyApp::onMouseLeftRelease(FI::Event e)
{
	float x = e.data_float()[0];
	float y = e.data_float()[1];
	FI::LOG("LMB release (", x, ",", y, ")");
}

void MyApp::onMouseRightClick(FI::Event e)
{
	float x = e.data_float()[0];
	float y = e.data_float()[1];
	FI::LOG("RMB click (", x, ",", y, ")");
}

void MyApp::onMouseRightRelease(FI::Event e)
{
	float x = e.data_float()[0];
	float y = e.data_float()[1];
	FI::LOG("RMB release (", x, ",", y, ")");
}

void MyApp::initScene()
{
	FI::LOG("initializing scene...");

	// setup geometry
	GLfloat vVertices[] = {
		 0.0f,  0.5f,  0.0f,   1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.0f,   0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f,  0.0f,   0.0f, 0.0f, 1.0f
	};

	// create VAO
#if defined(IOS) || defined(WEB)
	glGenVertexArraysOES(1, &m_vertexArrayObject);
	glBindVertexArrayOES(m_vertexArrayObject);
#else
	glGenVertexArrays(1, &m_vertexArrayObject);
	glBindVertexArray(m_vertexArrayObject);
#endif
	checkError(__PRETTY_FUNCTION__, "VAO");

	// Load the vertex data
	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	checkError(__PRETTY_FUNCTION__, "VBO");

	// setup shaders
	GLuint vshad = 0;
	GLuint fshad = 0;

	float mver = majorVersion() + minorVersion()/10.0f;
	bool bGLESFlag = false;

#if defined(IOS) || defined(WEB)
	bGLESFlag = true;
#endif

	if (mver >= 3)
	{
		vshad = compileShader(GL_VERTEX_SHADER, vShaderStrGL3);
		fshad = compileShader(GL_FRAGMENT_SHADER, fShaderStrGL3);
		FI::LOG("using GL 3 shaders");
	}
	else if (mver > 2 || bGLESFlag)
	{
		vshad = compileShader(GL_VERTEX_SHADER, vShaderStrGLES2);
		fshad = compileShader(GL_FRAGMENT_SHADER, fShaderStrGLES2);
		FI::LOG("using GL ES 2 shaders");
	}
	else
	{
		vshad = compileShader(GL_VERTEX_SHADER, vShaderStrGL2);
		fshad = compileShader(GL_FRAGMENT_SHADER, fShaderStrGL2);
		FI::LOG("using GL 2 shaders");
	}

	m_shaderProgram = buildShaderProgram(vshad, fshad);
	checkError(__PRETTY_FUNCTION__, "<build shader program>");

	// Use the program object
	glUseProgram(m_shaderProgram);

	// Set the viewport
	glViewport(0, 0, m_width, m_height);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glClearColor(0.0f, 0.2f, 0.1f, 1.0f);

	consumeError();
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

	GLsizei vSize = sizeof(GLfloat)*6;
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vSize, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vSize, (void*)(sizeof(GLfloat)*3));
	checkError(__PRETTY_FUNCTION__, "VertexAttribPointer");

	glEnableVertexAttribArray(0); // we bound 0 to vPosition earlier
	glEnableVertexAttribArray(1); // bound 1 to vColor

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	checkError(__PRETTY_FUNCTION__, "glDrawArrays");

#else
	// draw D3D9 stuff
#endif /* D3D */
}
