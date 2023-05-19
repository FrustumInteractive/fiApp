#include "fi/app/event.h"
#include <memory.h>
#include <algorithm>

using FI::Event;
using FI::EventListener;

FI::eEventType Event::type() { return m_type; }

void Event::setType( eEventType type ) { m_type = type; }

void Event::setData( float x, float y )
{
	memcpy(&m_byteData[0],&x,sizeof(float));
	memcpy(&m_byteData[4],&y,sizeof(float));
}
void Event::setData( unsigned int x, unsigned int y)
{
	memcpy(&m_byteData[0],&x,sizeof(unsigned int));
	memcpy(&m_byteData[4],&y,sizeof(unsigned int));
}
void Event::setData( uint64_t data )
{
	memcpy(m_byteData, &data, sizeof(uint64_t));
}
void Event::setData( const unsigned char *data )
{
	memcpy(m_byteData, data, sizeof(uint8_t)*8);
}
void Event::setData( double data )
{
	memcpy(m_byteData, &data, sizeof(double));
}

void Event::setSourceName( std::string name ) { m_sourceName = name; }

void Event::setSourceID( unsigned int id ) { m_sourceID = id; }
	
std::string Event::sourceName() { return m_sourceName; }

unsigned int Event::sourceID() { return m_sourceID; }

const float *Event::data_float() { return (const float*)m_byteData; }
const unsigned int *Event::data_uint32() { return (const unsigned int*)m_byteData; }
uint64_t Event::data_uint64() { return *(uint64_t*)m_byteData; }
const unsigned char *Event::data_ubyte() { return m_byteData; }
double Event::data_double() { return *(double*)m_byteData; }

// - EventListener

void EventListener::notify(FI::Event e)
{
	for(size_t i=0; i<m_eventListeners.size(); i++)
	{
		switch(e.type())
		{
			case EVENT_MOUSE_LEFT_CLICK:
				m_eventListeners[i]->onMouseLeftClick(e);
				break;
			case EVENT_MOUSE_LEFT_DRAG:
				m_eventListeners[i]->onMouseLeftDrag(e);
				break;
			case EVENT_MOUSE_LEFT_RELEASE:
				m_eventListeners[i]->onMouseLeftRelease(e);
				break;
			case EVENT_MOUSE_RIGHT_CLICK:
				m_eventListeners[i]->onMouseRightClick(e);
				break;
			case EVENT_MOUSE_RIGHT_DRAG:
				m_eventListeners[i]->onMouseRightDrag(e);
				break;
			case EVENT_MOUSE_RIGHT_RELEASE:
				m_eventListeners[i]->onMouseRightRelease(e);
				break;
			case EVENT_MOUSE_MOVE:
				m_eventListeners[i]->onMouseMove(e);
				break;
			case EVENT_HIDE:
				m_eventListeners[i]->onHide(e);
				break;
			case EVENT_MOVE:
				m_eventListeners[i]->onMove(e);
				break;
			case EVENT_SHOW:
				m_eventListeners[i]->onShow(e);
				break;
			case EVENT_REDRAW:
				m_eventListeners[i]->onRedraw(e);
				break;
			case EVENT_RESIZE:
				m_eventListeners[i]->onResize(e);
				break;
			case EVENT_UPDATE:
				m_eventListeners[i]->onUpdate(e);
				break;
			case EVENT_KEY_PRESS:
				m_eventListeners[i]->onKeyPress(e);
				break;
			case EVENT_KEY_RELEASE:
				m_eventListeners[i]->onKeyRelease(e);
				break;

			case EVENT_UI_VALUE_CHANGE:
				m_eventListeners[i]->onValueChange(e);
				break;
			case EVENT_UI_HOVER:
				m_eventListeners[i]->onHover(e);
				break;
			case EVENT_UI_CLICK:
				m_eventListeners[i]->onClick(e);
				break;
			case EVENT_UI_RELEASE:
				m_eventListeners[i]->onRelease(e);
				break;
			case EVENT_UI_DRAG:
				m_eventListeners[i]->onDrag(e);
				break;
			case EVENT_UI_RIGHT_CLICK:
				m_eventListeners[i]->onRightClick(e);
				break;
			case EVENT_UI_RIGHT_RELEASE:
				m_eventListeners[i]->onRightRelease(e);
				break;
			case EVENT_UI_RIGHT_DRAG:
				m_eventListeners[i]->onRightDrag(e);
				break;
			case EVENT_UI_MIDDLE_CLICK:
				m_eventListeners[i]->onMiddleClick(e);
				break;
			case EVENT_UI_MIDDLE_RELEASE:
				m_eventListeners[i]->onMiddleRelease(e);
				break;
			case EVENT_UI_MIDDLE_DRAG:
				m_eventListeners[i]->onMiddleDrag(e);
				break;
				
			default:
				break;
		}
	}
}

void EventListener::registerListener(FI::EventListener *listener)
{
	m_eventListeners.push_back(listener);
}

void EventListener::unregisterListener(FI::EventListener *listener)
{
	m_eventListeners.erase(
		std::remove(m_eventListeners.begin(), m_eventListeners.end(), listener), m_eventListeners.end()
	);
}
