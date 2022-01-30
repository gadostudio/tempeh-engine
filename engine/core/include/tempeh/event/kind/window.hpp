#ifndef _TEMPEH_EVENT_EVENT_WINDOW_HPP
#define _TEMPEH_EVENT_EVENT_WINDOW_HPP

class WindowResizeEvent : public Event
{
private:
	u32 width;
	u32 height;
public:
	WindowResize() {}
};

class WindowCloseEvent : public Event
{
private:
	Tempeh::Math::vec2 mouse_delta;
public:
	MouseMoved() {}

	TEMPEH_EVENT_CLASS_TYPE(MouseMoved)
};

#endif
