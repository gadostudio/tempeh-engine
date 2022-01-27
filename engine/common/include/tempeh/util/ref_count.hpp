#ifndef _TEMPEH_UTIL_REF_COUNT_H
#define _TEMPEH_UTIL_REF_COUNT_H

#include <atomic>

namespace Tempeh::Util
{
    class RefCount
    {
    public:
        RefCount() :
            m_counter(1)
        {
        }

        virtual ~RefCount()
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
}

#endif