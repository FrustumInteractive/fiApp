/*
* webApp.h
*
*  Created on: Sept 10, 2023
*      Author: Roger Dass
*		Copyright Frustum Interactive Inc. - All rights reserved.
*/

#include <stdlib.h>
#include <iostream>
#include <string>

#include "fi/app/webApp.h"

using namespace std;

void *_glContext = 0;
void *_glPixelFormat = 0;

std::function<void()> loop;
void main_loop()
{
	loop();
}

static eKeyCode sdlKeycodeToFIKey(SDL_Keycode key)
{
	if (key >= SDLK_a && key <= SDLK_z)
	{
		return (eKeyCode)((int)KEY_A + key - SDLK_a);
	}
	if (key >= SDLK_0 && key <= SDLK_9)
	{
		return (eKeyCode)((int)KEY_0 + key - SDLK_0);
	}
	if (key >= SDLK_F1 && key <= SDLK_F12)
	{
		return (eKeyCode)((int)KEY_F1 + key - SDLK_F1);
	}

	switch (key)
	{
		case SDLK_SPACE: return KEY_SPACE;
		case SDLK_ESCAPE: return KEY_ESC;
		case SDLK_BACKSPACE: return KEY_BS;
		case SDLK_TAB: return KEY_TAB;
		case SDLK_RETURN: return KEY_ENTER;
		case SDLK_LSHIFT:
		case SDLK_RSHIFT: return KEY_SHIFT;
		case SDLK_LCTRL:
		case SDLK_RCTRL: return KEY_CTRL;
		case SDLK_LALT:
		case SDLK_RALT: return KEY_ALT;
		case SDLK_INSERT: return KEY_INS;
		case SDLK_DELETE: return KEY_DEL;
		case SDLK_HOME: return KEY_HOME;
		case SDLK_END: return KEY_END;
		case SDLK_PAGEUP: return KEY_PAGEUP;
		case SDLK_PAGEDOWN: return KEY_PAGEDOWN;
		case SDLK_UP: return KEY_UP;
		case SDLK_DOWN: return KEY_DOWN;
		case SDLK_LEFT: return KEY_LEFT;
		case SDLK_RIGHT: return KEY_RIGHT;
		case SDLK_BACKQUOTE: return KEY_TILDA;
		case SDLK_MINUS: return KEY_MINUS;
		case SDLK_EQUALS: return KEY_PLUS;
		case SDLK_LEFTBRACKET: return KEY_LBRACKET;
		case SDLK_RIGHTBRACKET: return KEY_RBRACKET;
		case SDLK_BACKSLASH: return KEY_BACKSLASH;
		case SDLK_SEMICOLON: return KEY_SEMICOLON;
		case SDLK_QUOTE: return KEY_SINGLEQUOTE;
		case SDLK_COMMA: return KEY_COMMA;
		case SDLK_PERIOD: return KEY_DOT;
		case SDLK_SLASH: return KEY_SLASH;
		default: return KEY_NULL;
	}
}

WebApp::WebApp(const int argc, const char *argv[]) :
	Application(argc, argv)
{

}

WebApp::~WebApp()
{
}

void WebApp::createWindow(const char *title, int x, int y, int width, int height, bool fullscreen)
{
	FI::LOG("WebApp::createWindow begin title=", title, " size=", width, "x", height, " fullscreen=", fullscreen);
	m_xpos = x;
	m_ypos = y;
	m_width = width;
	m_height = height;
	m_bFullscreen = fullscreen;

	int sdlInitRet = SDL_Init(SDL_INIT_VIDEO);
	FI::LOG("SDL_Init(SDL_INIT_VIDEO) ret=", sdlInitRet);

#if defined(FI_GFX_WEBGPU)
	mWindow = SDL_CreateWindow(
		title,
		m_xpos,
		m_ypos,
		m_width,
		m_height,
		SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);

	if (!mWindow)
	{
		FI::LOG("Failed to create SDL window for WebGPU");
		return;
	}
	FI::LOG("SDL window created for WebGPU");

	FI::LOG("WebGPU window ready (device init is handled by app)");
	SDL_GetWindowSize(mWindow, &m_width, &m_height);
	if (m_width < 1 || m_height < 1)
	{
		m_width = width;
		m_height = height;
	}
	FI::LOG("WebGPU drawable size=", m_width, "x", m_height);
#else
#ifdef _GLES3
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_CreateWindowAndRenderer(m_width, m_height,
		SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE,
		&mWindow, nullptr);

	SDL_GL_CreateContext(mWindow);

	const GLubyte* glVersion = glGetString(GL_VERSION);
	printf("GLVersion = %s\n", glVersion);
	
	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	printf("vendor = %s\nrenderer = %s\n", vendor, renderer);

	SDL_GL_GetDrawableSize(mWindow, &m_width, &m_height);
#endif

	gfxAPIInit();
	FI::LOG("WebApp::gfxAPIInit completed");
	SDL_SetWindowInputFocus(mWindow);

	loop = [&]()
	{
		mainloop();
	};

	emscripten_set_main_loop(main_loop, 0, true);
	FI::LOG("emscripten_set_main_loop registered");
}

void WebApp::destroyWindow()
{
	FI::LOG("WebApp::destroyWindow");
	gfxAPIDeinit();

	//TODO: cleanup SDL stuff
}

void WebApp::mainloop()
{
	syncDrawableSize();

	SDL_Event ev;
	while (SDL_PollEvent(&ev))
	{
		FI::Event e;
		switch (ev.type)
		{
			case SDL_MOUSEBUTTONDOWN:
			{
				SDL_SetWindowInputFocus(mWindow);
				if (ev.button.button == SDL_BUTTON_LEFT)
				{
					m_leftBtnDown = true;
					e.setType(FI::EVENT_MOUSE_LEFT_CLICK);
				}
				else if (ev.button.button == SDL_BUTTON_MIDDLE)
				{
					m_middleBtnDown = true;
					e.setType(FI::EVENT_MOUSE_MIDDLE_CLICK);
				}
				else if (ev.button.button == SDL_BUTTON_RIGHT)
				{
					m_rightBtnDown = true;
					e.setType(FI::EVENT_MOUSE_RIGHT_CLICK);
				}
				else
				{
					break;
				}
				e.setData((float)ev.button.x, (float)ev.button.y);
				setEvent(e);
				break;
			}

			case SDL_MOUSEBUTTONUP:
			{
				if (ev.button.button == SDL_BUTTON_LEFT)
				{
					m_leftBtnDown = false;
					e.setType(FI::EVENT_MOUSE_LEFT_RELEASE);
				}
				else if (ev.button.button == SDL_BUTTON_MIDDLE)
				{
					m_middleBtnDown = false;
					e.setType(FI::EVENT_MOUSE_MIDDLE_RELEASE);
				}
				else if (ev.button.button == SDL_BUTTON_RIGHT)
				{
					m_rightBtnDown = false;
					e.setType(FI::EVENT_MOUSE_RIGHT_RELEASE);
				}
				else
				{
					break;
				}
				e.setData((float)ev.button.x, (float)ev.button.y);
				setEvent(e);
				break;
			}

			case SDL_MOUSEMOTION:
			{
				if (m_leftBtnDown)
				{
					e.setType(FI::EVENT_MOUSE_LEFT_DRAG);
				}
				else if (m_middleBtnDown)
				{
					e.setType(FI::EVENT_MOUSE_MIDDLE_DRAG);
				}
				else if (m_rightBtnDown)
				{
					e.setType(FI::EVENT_MOUSE_RIGHT_DRAG);
				}
				else
				{
					e.setType(FI::EVENT_MOUSE_MOVE);
				}
				e.setData(
					(float)ev.motion.x,
					(float)ev.motion.y,
					(float)ev.motion.xrel,
					(float)ev.motion.yrel);
				setEvent(e);
				break;
			}

			case SDL_MOUSEWHEEL:
			{
				// Positive y means wheel up; match native path semantics (+1 up, -1 down).
				float wheelDelta = (float)ev.wheel.y;
				if (ev.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
				{
					wheelDelta = -wheelDelta;
				}
				if (wheelDelta != 0.0f)
				{
					e.setType(FI::EVENT_MOUSE_WHEEL);
					e.setData(wheelDelta, 0.0f);
					setEvent(e);
				}
				break;
			}

			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{
				if (ev.key.repeat)
				{
					break;
				}
				eKeyCode kc = sdlKeycodeToFIKey(ev.key.keysym.sym);
				if (kc != KEY_NULL)
				{
					e.setType(ev.type == SDL_KEYDOWN ? FI::EVENT_KEY_PRESS : FI::EVENT_KEY_RELEASE);
					e.setData((unsigned int)kc, 0);
					setEvent(e);
				}
				break;
			}

			case SDL_WINDOWEVENT:
			{
				if (ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
					ev.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					syncDrawableSize();
				}
				break;
			}

			case SDL_QUIT:
				m_bQuit = true;
				break;

			default:
				break;
		}
	}

	gfxAPIDraw(); // our draw call
#if !defined(FI_GFX_WEBGPU)
	swapBuffers();
#endif
}

void WebApp::setRelativeMouseMode(bool enabled)
{
	m_relativeMouseMode = enabled;
	if (enabled)
	{
		emscripten_request_pointerlock("#canvas", true);
	}
	else
	{
		emscripten_exit_pointerlock();
	}
	SDL_SetRelativeMouseMode(enabled ? SDL_TRUE : SDL_FALSE);
}

void WebApp::syncDrawableSize()
{
	if (!mWindow)
	{
		return;
	}

	int drawableW = 0;
	int drawableH = 0;
#if defined(FI_GFX_WEBGPU)
	SDL_GetWindowSize(mWindow, &drawableW, &drawableH);
#else
	SDL_GL_GetDrawableSize(mWindow, &drawableW, &drawableH);
#endif
	if (drawableW > 0 && drawableH > 0 && (drawableW != m_width || drawableH != m_height))
	{
		m_width = drawableW;
		m_height = drawableH;
		resize(m_width, m_height);
	}
}

void WebApp::swapBuffers()
{
#if !defined(FI_GFX_WEBGPU)
	SDL_GL_SwapWindow(mWindow);
#endif
}
