/*
* Application.cpp
*
*  Created on: Aug 10, 2015
*      Author: Roger Dass
*		Copyright Frustum Interactive Inc. - All rights reserved.
*/

#include "fi/app/app.h"
#include "fi/app/fileIO.h"

Application::Application(const int argc, const char *argv[])
	: m_title("Application")
	, m_executiblePath("")
	, m_documentPath("")
{
	// store commandline arguments into our own vector
	if (argc > 1)
	{
		for (int i = 1; i < argc; i++)
		{
			m_cmdLineArgs.push_back(std::string(argv[i]));
		}
	}

	registerListener(this);
}

std::string Application::cmdLineArg(std::string option, bool hasValue)
{
	std::string retVal = "";
	for (size_t i=0; i<m_cmdLineArgs.size(); i++)
	{
		if (m_cmdLineArgs[i] == option)
		{
			if(hasValue)
			{
				if (m_cmdLineArgs.size() > i + 1)
				{
					retVal = m_cmdLineArgs[i + 1];
				}
			}
			else retVal = m_cmdLineArgs[i];
		}
		if (!retVal.empty()) break;
	}
	return retVal;
}

std::vector<std::string> Application::filesInDirectory(std::string dirPath, std::string ext)
{
	return FI::filesInDirectory(dirPath, ext);
}

void Application::setEvent(FI::Event e)
{
	switch (e.type())
	{
		case FI::EVENT_MOUSE_LEFT_CLICK:
		case FI::EVENT_MOUSE_LEFT_DRAG:
		case FI::EVENT_MOUSE_LEFT_RELEASE:
		case FI::EVENT_MOUSE_RIGHT_CLICK:
		case FI::EVENT_MOUSE_RIGHT_DRAG:
		case FI::EVENT_MOUSE_RIGHT_RELEASE:
		case FI::EVENT_MOUSE_MOVE:
		{
			// normalize to opengl coords
			float x = e.data_float()[0] / m_width * m_scaleFactor;
			float y = 1.0f - e.data_float()[1] / m_height * m_scaleFactor;
			e.setData(x,y);
		}
			break;

		default:
			break;
	}

	notify(e);
}
