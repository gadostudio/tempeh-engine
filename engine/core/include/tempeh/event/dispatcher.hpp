#ifndef _TEMPEH_EVENT_DISPATCHER_HPP
#define _TEMPEH_EVENT_DISPATCHER_HPP

namespace Tempeh::Event
{

	class Dispatcher
	{
	private:
		Event& event;
	public:
		Dispatcher(Event& event) : event(event) {}
		template <typename T, typename = std::enable_if_t<std::is_base_of_v<Event, T>>>
		void dispatch() {}
	};

}

#endif
