#ifndef _TEMPEH_EVENT_EVENT_MOUSE_HPP
#define _TEMPEH_EVENT_EVENT_MOUSE_HPP

class MouseButtonEvent : public Event
{
private:

public:
	MouseButton() {}
};

class MouseMovedEvent : public Event
{
private:
	Tempeh::Math::vec2 mouse_delta;
public:
	MouseMoved() {}

	TEMPEH_EVENT_CLASS_TYPE(MouseMoved)
};

#endif
