#ifndef _TEMPEH_CORE_TIMESTEP_HPP
#define _TEMPEH_CORE_TIMESTEP_HPP

#include <typedefs.hpp>

class Duration
{
private:
	f32 m_secs;
public:
	Duration(f32 time_secs) : m_secs(time_secs) {};

	f32 as_secs() { return m_secs; }
	f32 as_milis() { return m_secs / 1000000.0f; }
	f32 as_nanos() { return m_secs / 1000000000.0f; }
};

#endif
