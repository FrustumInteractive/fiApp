#ifndef _EVENT_H
#define _EVENT_H

#include <string>
#include <vector>
#include <cstdint>

namespace FI {

enum eEventType
{
	EVENT_NONE,
	EVENT_RESIZE,
	EVENT_MOVE,
	EVENT_SHOW,
	EVENT_HIDE,
	EVENT_REDRAW,
	EVENT_UPDATE,
	EVENT_FADE_IN,
	EVENT_FADE_OUT,
	EVENT_MOUSE_MOVE,
	EVENT_MOUSE_LEFT_CLICK,
	EVENT_MOUSE_LEFT_RELEASE,
	EVENT_MOUSE_LEFT_DRAG,
	EVENT_MOUSE_MIDDLE_CLICK,
	EVENT_MOUSE_MIDDLE_RELEASE,
	EVENT_MOUSE_MIDDLE_DRAG,
	EVENT_MOUSE_RIGHT_CLICK,
	EVENT_MOUSE_RIGHT_RELEASE,
	EVENT_MOUSE_RIGHT_DRAG,
	EVENT_KEY_PRESS,
	EVENT_KEY_RELEASE,
	EVENT_UI_VALUE_CHANGE,
	EVENT_UI_CLICK,
	EVENT_UI_RELEASE,
	EVENT_UI_DRAG,
	EVENT_UI_RIGHT_CLICK,
	EVENT_UI_RIGHT_RELEASE,
	EVENT_UI_RIGHT_DRAG,
	EVENT_UI_MIDDLE_CLICK,
	EVENT_UI_MIDDLE_RELEASE,
	EVENT_UI_MIDDLE_DRAG,
	EVENT_UI_HOVER

};

class Event
{
public:
	eEventType type();
	void setType( eEventType type );

	void setData( float x, float y);
	void setData( uint64_t data );
	void setData( unsigned int x, unsigned int y);
	void setData( const double data );
	void setData( const unsigned char *data );

	void setSourceName( std::string name );
	void setSourceID( unsigned int id );

	std::string sourceName();

	unsigned int sourceID();

	const float *data_float();
	double data_double();
	uint64_t data_uint64();
	const unsigned int *data_uint32();
	const unsigned char *data_ubyte();
	
protected:
	eEventType m_type;

	unsigned char m_byteData[8];

	std::string m_sourceName;
	unsigned int m_sourceID;
};

class EventListener
{
public:

	void notify(FI::Event e);
	void registerListener(EventListener* listener);
	void unregisterListener(EventListener* listener);

	// window
	virtual void onResize(FI::Event e){}
	virtual void onMove(FI::Event e){}
	virtual void onShow(FI::Event e){}
	virtual void onHide(FI::Event e){}
	virtual void onRedraw(FI::Event e){}
	virtual void onUpdate(FI::Event e){}

	// mouse / touch
	virtual void onMouseMove(FI::Event e){}
	virtual void onMouseLeftClick(FI::Event e){}
	virtual void onMouseLeftRelease(FI::Event e){}
	virtual void onMouseLeftDrag(FI::Event e){}
	virtual void onMouseRightClick(FI::Event e){}
	virtual void onMouseRightRelease(FI::Event e){}
	virtual void onMouseRightDrag(FI::Event e){}
	virtual void onMouseMiddleClick(FI::Event e){}
	virtual void onMouseMiddleRelease(FI::Event e){}
	virtual void onMouseMiddleDrag(FI::Event e){}

	// keyboard
	virtual void onKeyPress(FI::Event e){}
	virtual void onKeyRelease(FI::Event e){}

	// ui widgets
	virtual void onClick(FI::Event e){}
	virtual void onRelease(FI::Event e){}
	virtual void onDrag(FI::Event e){}
	virtual void onRightClick(FI::Event e){}
	virtual void onRightRelease(FI::Event e){}
	virtual void onRightDrag(FI::Event e){}
	virtual void onMiddleClick(FI::Event e){}
	virtual void onMiddleRelease(FI::Event e){}
	virtual void onMiddleDrag(FI::Event e){}
	virtual void onHover(FI::Event e){}
	virtual void onValueChange(FI::Event e){}

private:

	std::vector<FI::EventListener*> m_eventListeners;
};

} /* namespace FI */

#endif /*_EVENT_H*/
