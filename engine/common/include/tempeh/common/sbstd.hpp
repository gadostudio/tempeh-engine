#include <cassert>

namespace sbstd {

	template <typename T, typename E>
	class Error
	{
	private:
		bool m_is_ok;
		union Inner
		{
			T ok;
			E error;
		};
		Inner m_inner;
	public:
		Error(T data) : m_is_ok(true)
		{
			m_inner.ok = std::move(data);
		};
		Error(E error) : m_is_ok(false)
		{
			m_inner.error = std::move(error);
		};

		bool is_ok() const { return m_is_ok; }
		const T& value() const { assert(m_is_ok); return m_inner.ok; }
		T& value_mut() { assert(m_is_ok); return m_inner.ok; }
		const E& error() const { assert(!m_is_ok); return m_inner.error; }
	};

	template <typename T>
	class Optional
	{
	private:
		bool m_is_some;
		T m_inner;
	public:
		Optional(T data) : is_some(true)
		{
			m_inner = std::move(data);
		};
		Optional(): is_some(false) {};

		bool is_some() const { return m_is_some; }
		const T& value() const { assert(m_is_some); return m_inner; }
		T& value_mut() const { assert(m_is_some); return m_inner; }
	};

}
