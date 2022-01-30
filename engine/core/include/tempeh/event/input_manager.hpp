#ifndef _TEMPEH_EVENT_INPUT_MANAGER_HPP
#define _TEMPEH_EVENT_INPUT_MANAGER_HPP

#include <tempeh/event/event.hpp>

#include <type_traits>
#include <set>

namespace Tempeh::Event
{

	template<typename T>
	class InputManager
	{
	private:
		std::set<T> m_keypressed;
	public:
		bool is_key_pressed(T key) {  };
	};

}

#endif
