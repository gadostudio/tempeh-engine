#ifndef TEMPEH_ENGINE_ENGINE_COLLECTION_CAPPED_VECTOR_HPP
#define TEMPEH_ENGINE_ENGINE_COLLECTION_CAPPED_VECTOR_HPP

#include <vector>
#include <iterator>
#include <cmath>

namespace sbstd::collections
{

    template <typename T>
    class CappedVector: public std::iterator<
        std::output_iterator_tag,
        T,
        T,
        const T*,
        T
    >
    {
    private:
        using size_type = typename std::vector<T>::size_type;
        using iterator = typename std::vector<T>::iterator;
        std::vector<T> m_data;
        size_type m_preserve_capacity;
        size_type m_max_size;
    public:
        CappedVector(size_type max_size, size_type preserve):
            m_preserve_capacity(preserve),
            m_max_size(max_size)
        {
            m_data.reserve(max_size + m_preserve_capacity);
        }

        iterator begin()
        {
            bool is_greater_than_size = m_data.size() > m_max_size;
            return m_data.begin() + (is_greater_than_size ? m_data.size() - m_max_size : 0);
        }
        iterator end()
        {
            return m_data.end();
        }
        void push_back(T& data)
        {
            if (m_data.size() == m_max_size + m_preserve_capacity)
            {
                m_data.erase(m_data.begin(), m_data.begin() + m_preserve_capacity);
            }
            m_data.push_back(std::move(data));
        }

        const size_type size()
        {
            return std::min(m_max_size, m_data.size());
        }

        const std::vector<T>& raw_data()
        {
            return m_data;
        }
    };

}

#endif //TEMPEH_ENGINE_ENGINE_COLLECTION_CAPPED_VECTOR_HPP
