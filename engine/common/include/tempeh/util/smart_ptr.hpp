#ifndef _TEMPEH_UTIL_REF_COUNT_H
#define _TEMPEH_UTIL_REF_COUNT_H

#include <atomic>
#include <memory>

namespace Tempeh::Util
{
    // Atomically Reference Counted
    class Arc
    {
    public:
        Arc() :
            m_counter(1)
        {
        }

        virtual ~Arc()
        {
        }

        void add_ref() const
        {
            m_counter.fetch_add(1, std::memory_order_relaxed);
        }

        void release() const
        {
            if (m_counter.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                delete this;
            }
        }

        uint32_t get_refs() const
        {
            return m_counter.load(std::memory_order_relaxed);
        }

    private:
        mutable std::atomic<uint32_t> m_counter;
    };

    template<class T>
    static inline T* safe_add_ref(T* ptr)
    {
        if (ptr != nullptr) {
            ptr->add_ref();
        }

        return ptr;
    }

    template<class T>
    static inline T* safe_release(T* ptr)
    {
        if (ptr != nullptr) {
            ptr->release();
        }

        return ptr;
    }

    // Reference Counted
    template<typename T>
    using Rc = std::shared_ptr<T>;

    template<typename T>
    using Box = std::unique_ptr<T>;

    template<typename T>
    FUNCTION_ALIAS(make_rc, std::make_shared<T>);
    template<typename T>
    FUNCTION_ALIAS(make_box, std::make_unique<T>);

}

#endif