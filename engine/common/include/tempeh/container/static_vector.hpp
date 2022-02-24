#ifndef _TEMPEH_CONTAINER_STATIC_ARRAY_HPP
#define _TEMPEH_CONTAINER_STATIC_ARRAY_HPP

#include <tempeh/common/traits.hpp>
#include <cassert>
#include <array>
#include <algorithm>
#include <utility>
#include <cstring>
#include <iterator>

namespace Tempeh::Container
{
    template<typename T, std::size_t N>
    class StaticVectorIterator
    {

    };

    template<typename T, std::size_t N>
    class StaticVector
    {
    public:
        static constexpr std::size_t max_elements = N;

        using ValueType = T;
        using SizeType = std::size_t;
        using DifferenceType = std::ptrdiff_t;
        using Reference = ValueType&;
        using ConstReference = const ValueType&;
        using Pointer = ValueType*;
        using ConstPointer = const ValueType*;
        using Iterator = Pointer;
        using ConstIterator = ConstPointer;

        StaticVector() : m_size(0) {}

        StaticVector(std::size_t num_elements) :
            m_size(num_elements)
        {
            assert(num_elements <= max_elements && "Too many elements");

            if constexpr (!std::is_trivially_default_constructible_v<T>) {
                std::uninitialized_default_construct_n(begin(), num_elements);
            }
        }

        StaticVector(std::size_t num_elements, const T& value) :
            m_size(num_elements)
        {
            assert(num_elements <= max_elements && "Too many elements");
            std::uninitialized_fill_n(begin(), num_elements, value);
        }

        StaticVector(std::initializer_list<T> list) :
            m_size(list.size())
        {
            assert(m_size <= max_elements && "Too many elements");
            std::uninitialized_move(list.begin(), list.end(), begin());
        }

        StaticVector(const StaticVector& other) :
            m_size(other.m_size)
        {
            std::uninitialized_copy(other.begin(), other.end(), begin());
        }

        template<std::size_t OtherN, Requires<OtherN <= max_elements> = true>
        StaticVector(const StaticVector<T, OtherN>& other) :
            m_size(other.m_size)
        {
            std::uninitialized_copy(other.begin(), other.end(), begin());
        }

        ~StaticVector()
        {
            destroy_all();
        }

        template<typename InputIt>
        void assign(InputIt first, InputIt last)
        {
            if (m_size == 0) {
                for (; first != last; ++first) {
                    assert(m_size < max_elements && "Too many elements");
                    ::new(data() + m_size) T(*first);
                    ++m_size;
                }
                return;
            }

            std::size_t new_size = 0;
            auto dst_first = begin();

            // Assign to existing elements
            for (; first != last; ++first, ++new_size) {
                if (new_size >= m_size) {
                    break;
                }

                *dst_first++ = *first;
            }

            if (new_size < m_size) {
                // Shrink
                if constexpr (!std::is_trivially_destructible_v<T>) {
                    std::destroy(begin() + new_size, begin() + m_size);
                }
                
                clear_region(begin() + new_size, begin() + m_size);
            }
            else {
                // Expand
                auto storage = data();
                for (; first != last; ++first) {
                    assert(new_size < max_elements && "Too many elements");
                    ::new(storage + new_size) T(*first); // construct new data
                    ++new_size;
                }
            }

            m_size = new_size;
            return;
        }

        void assign(std::initializer_list<T> list)
        {
            assign(list.begin(), list.end());
        }

        StaticVector& operator=(const StaticVector& other)
        {
            if (this == std::addressof(other)) {
                return *this;
            }

            assign(other.begin(), other.end());

            return *this;
        }

        StaticVector& operator=(std::initializer_list<T> list)
        {
            assign(list.begin(), list.end());
            return *this;
        }

        T& at(std::size_t index)
        {
            assert(index < m_size && "Index out of range");
            return reinterpret_cast<T&>(m_storage[index]);
        }

        const T& at(std::size_t index) const
        {
            assert(index < m_size && "Index out of range");
            return reinterpret_cast<const T&>(m_storage[index]);
        }

        T& operator[](std::size_t index)
        {
            assert(index < m_size && "Index out of range");
            return reinterpret_cast<T&>(m_storage[index]);
        }

        const T& operator[](std::size_t index) const
        {
            assert(index < m_size && "Index out of range");
            return reinterpret_cast<const T&>(m_storage[index]);
        }

        T* data() noexcept { return reinterpret_cast<T*>(m_storage.data()); }
        const T* data() const noexcept { return reinterpret_cast<const T*>(m_storage.data()); }

        void push_back(const T& value)
        {
            assert(m_size < max_elements && "Storage is full");
            new(end()) T(value);
            m_size++;
        }

        void push_back(T&& value)
        {
            assert(m_size < max_elements && "Storage is full");
            new(end()) T(std::move(value));
            m_size++;
        }

        template<typename... Args>
        T& emplace_back(Args&&... args)
        {
            assert(m_size < max_elements && "Storage is full");

            if constexpr (std::is_trivially_constructible_v<T>) {
                ::new(end()) T{std::forward<Args>(args)...};
            }
            else {
                ::new(end()) T(std::forward<Args>(args)...);
            }

            return (*this)[m_size++];
        }

        void pop_back()
        {
            if (m_size == 0) {
                return;
            }

            m_size--;

            if constexpr (!std::is_trivially_destructible_v<T>) {
                std::destroy_at(end());
            }
            else {
                *end() = {};
            }
        }

        void resize(std::size_t n)
        {
            assert(n <= max_elements && "Too many elements");

            if (n == 0) {
                destroy_all();
                m_size = 0;
                return;
            }

            // Expand
            if (n > m_size) {
                if constexpr (!std::is_trivially_default_constructible_v<T>) {
                    auto expand_first = data() + m_size;
                    auto expand_last = data() + n;

                    // Initialize expanded region
                    std::uninitialized_default_construct(expand_first, expand_last);
                }
            }
            // Trim
            else if (n < m_size) {
                auto trim_first = data() + n;
                auto trim_last = data() + m_size;

                // We don't have to call the destructor if T can be "trivially" destructed
                if constexpr (!std::is_trivially_destructible_v<T>) {
                    // Destroy trimmed region
                    std::destroy(trim_first, trim_last);
                }

                clear_region(trim_first, trim_last);
            }

            m_size = n;
        }

        Iterator begin() noexcept { return data(); }
        ConstIterator begin() const noexcept { return data(); }
        ConstIterator cbegin() const noexcept { return data(); }

        Iterator end() noexcept { return data() + m_size; }
        ConstIterator end() const noexcept { return data() + m_size; }
        ConstIterator cend() const noexcept { return data() + m_size; }

        bool empty() const
        {
            return begin() == end();
        }

        std::size_t size() const
        {
            return m_size;
        }

    private:
        using StorageType = std::aligned_storage_t<sizeof(T), alignof(T)>;

        union ValueStorage
        {
            T value;
            StorageType storage;

            ValueStorage() :
                storage()
            {
            }

            ~ValueStorage()
            {
            }
        };

        std::array<ValueStorage, N> m_storage{};
        std::size_t m_size;

        void destroy_all()
        {
            if constexpr (!std::is_trivially_destructible_v<T>) {
                for (auto& elem : *this) {
                    elem.~T();
                }
            }
        }

        template<typename InputIt>
        void clear_region(InputIt first, InputIt last)
        {
            std::memset(first, 0, sizeof(StorageType) * (last - first));
        }
    };
}

#endif