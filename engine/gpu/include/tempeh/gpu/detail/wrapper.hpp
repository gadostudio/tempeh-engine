#ifndef _TEMPEH_GPU_DETAIL_WRAPPER_HPP
#define _TEMPEH_GPU_DETAIL_WRAPPER_HPP

#include <memory>
#include <type_traits>
#include <tempeh/util/ref_count.hpp>

namespace Tempeh::GPU::Detail
{
    template<typename T, std::enable_if_t<std::is_base_of_v<Util::RefCount, T>, bool> = true>
    class Wrapper
    {
    public:
        Wrapper(T* impl) :
            m_impl(impl)
        {
        }

        Wrapper(const Wrapper<T>& other) :
            m_impl(safe_add_ref(other.m_impl))
        {
        }

        ~Wrapper()
        {
            safe_release(m_impl);
        }

        Wrapper& operator=(const Wrapper<T>& other)
        {
            if (m_impl != other.m_impl) {
                reset(safe_add_ref(other.m_impl));
            }

            return *this;
        }

    protected:
        T* m_impl;

        void reset(T* new_ptr)
        {
            T* tmp = m_impl;
            m_impl = new_ptr;
            safe_release(tmp);
        }
    };
}

#endif