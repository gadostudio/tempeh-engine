#ifndef _TEMPEH_EVENT_DISPATCHER_HPP
#define _TEMPEH_EVENT_DISPATCHER_HPP

#include <vector>
#include <utility>
#include <functional>

#include <tempeh/input//event.hpp>

namespace Tempeh::Input
{

	class Dispatcher
	{
	public:
		using DispatchCallback = std::function<void(const Tempeh::Input::Event&)>;
		using DispatchCallbacksWithType = std::vector<std::pair<Type, DispatchCallback>>;

		template<Type T = Type::None>
		void dispatch(DispatchCallback f)
		{
			m_dispatch_callbacks.push_back(std::make_pair(T, f));
		}

		const DispatchCallbacksWithType& get_dispatch_callbacks()
		{
			return m_dispatch_callbacks;
		}

	private:
		DispatchCallbacksWithType m_dispatch_callbacks;
	};

}

#endif
