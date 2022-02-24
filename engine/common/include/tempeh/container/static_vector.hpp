#ifndef _TEMPEH_CONTAINER_STATIC_ARRAY_HPP
#define _TEMPEH_CONTAINER_STATIC_ARRAY_HPP

#include <tempeh/common/traits.hpp>
#include <cassert>
#include <array>
#include <algorithm>
#include <memory>
#include <utility>
#include <cstring>
#include <iterator>

namespace Tempeh::Container
{
    template<typename VecType>
    class StaticVectorConstIterator
    {
    public:
        using Self = StaticVectorConstIterator<VecType>;

        using DifferenceType = typename VecType::DifferenceType;
        using ValueType = typename VecType::ValueType;
        using Pointer = typename VecType::ConstPointer;
        using Reference = typename VecType::ConstReference;
        using IteratorCategory = std::random_access_iterator_tag;

        using difference_type = DifferenceType;
        using value_type = ValueType;
        using pointer = Pointer;
        using reference = Reference;
        using iterator_category = IteratorCategory;
        
        StaticVectorConstIterator(const Pointer ptr) :
            current_ptr(ptr)
        {
        }

        Reference operator*() const
        {
            return *current_ptr;
        }

        Pointer operator->() const
        {
            return current_ptr;
        }

        Self& operator++()
        {
            current_ptr++;
            return *this;
        }

        Self operator++(int)
        {
            Self tmp = *this;
            ++*this;
            return tmp;
        }

        Self& operator--()
        {
            current_ptr--;
            return *this;
        }

        Self operator--(int)
        {
            Self tmp = *this;
            --*this;
            return tmp;
        }

        Self& operator+=(std::ptrdiff_t offset)
        {
            current_ptr += offset;
            return *this;
        }

        Self& operator-=(std::ptrdiff_t offset)
        {
            current_ptr -= offset;
            return *this;
        }

        Self operator+(std::ptrdiff_t offset) const
        {
            return Self(current_ptr + offset);
        }

        Self operator-(std::ptrdiff_t offset) const
        {
            return Self(current_ptr - offset);
        }

        std::ptrdiff_t operator-(const Self& other) const
        {
            return current_ptr - other.current_ptr;
        }

        Reference operator[](std::ptrdiff_t offset) const
        {
            return current_ptr[offset];
        }

        bool operator==(const StaticVectorConstIterator& other) const { return current_ptr == other.current_ptr; }
        bool operator!=(const StaticVectorConstIterator& other) const { return !(*this == other); }
        bool operator<(const StaticVectorConstIterator& other) const { return current_ptr < other.current_ptr; }
        bool operator>(const StaticVectorConstIterator& other) const { return current_ptr > other.current_ptr; }
        bool operator<=(const StaticVectorConstIterator& other) const { return current_ptr <= other.current_ptr; }
        bool operator>=(const StaticVectorConstIterator& other) const { return current_ptr >= other.current_ptr; }

    protected:
        Pointer current_ptr;
    };

    template<typename VecType>
    class StaticVectorIterator : public StaticVectorConstIterator<VecType>
    {
    public:
        using Self = StaticVectorIterator<VecType>;
        using Base = StaticVectorConstIterator<VecType>;

        using DifferenceType = typename VecType::DifferenceType;
        using ValueType = typename VecType::ValueType;
        using Pointer = typename VecType::Pointer;
        using Reference = typename VecType::Reference;
        using IteratorCategory = std::random_access_iterator_tag;

        using difference_type = DifferenceType;
        using value_type = ValueType;
        using pointer = Pointer;
        using reference = Reference;
        using iterator_category = IteratorCategory;

        StaticVectorIterator(Pointer ptr) :
            Base(ptr)
        {
        }

        Reference operator*()
        {
            return const_cast<Reference>(Base::operator*());
        }

        Pointer operator->()
        {
            return const_cast<Pointer>(Base::current_ptr);
        }

        StaticVectorIterator& operator++()
        {
            Base::operator++();
            return *this;
        }

        StaticVectorIterator operator++(int)
        {
            StaticVectorIterator tmp = *this;
            Base::operator++();
            return tmp;
        }

        StaticVectorIterator& operator--()
        {
            Base::operator--();
            return *this;
        }

        StaticVectorIterator operator--(int)
        {
            StaticVectorIterator tmp = *this;
            Base::operator--();
            return tmp;
        }

        StaticVectorIterator& operator+=(std::ptrdiff_t offset)
        {
            Base::operator+=(offset);
            return *this;
        }

        StaticVectorIterator& operator-=(std::ptrdiff_t offset)
        {
            Base::operator-=(offset);
            return *this;
        }

        StaticVectorIterator operator+(std::ptrdiff_t offset) const
        {
            return StaticVectorIterator(const_cast<Pointer>(Base::current_ptr) + offset);
        }

        StaticVectorIterator operator-(std::ptrdiff_t offset) const
        {
            return StaticVectorIterator(const_cast<Pointer>(Base::current_ptr) - offset);
        }

        std::ptrdiff_t operator-(const Self& other) const
        {
            return Base::current_ptr - other.current_ptr;
        }

        Pointer operator[](std::ptrdiff_t offset)
        {
            return *(*this + offset);
        }  
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
        using Iterator = StaticVectorIterator<StaticVector<T, N>>;
        using ConstIterator = StaticVectorConstIterator<StaticVector<T, N>>;

        using value_type = ValueType;
        using size_type = SizeType;
        using difference_type = DifferenceType;
        using reference = Reference;
        using const_reference = ConstReference;
        using pointer = Pointer;
        using const_pointer = ConstPointer;
        using iterator = Iterator;
        using const_iterator = ConstIterator;

        StaticVector() : m_size(0) {}

        StaticVector(std::size_t num_elements) :
            m_size(num_elements)
        {
            assert(num_elements <= max_elements && "Too many elements");

            if constexpr (!std::is_trivially_default_constructible_v<T>) {
                std::uninitialized_default_construct_n(data(), num_elements);
            }
        }

        StaticVector(std::size_t num_elements, const T& value) :
            m_size(num_elements)
        {
            assert(num_elements <= max_elements && "Too many elements");
            std::uninitialized_fill_n(data(), num_elements, value);
        }

        StaticVector(std::initializer_list<T> list) :
            m_size(list.size())
        {
            assert(m_size <= max_elements && "Too many elements");
            std::uninitialized_move(list.begin(), list.end(), data());
        }

        StaticVector(const StaticVector& other) :
            m_size(other.m_size)
        {
            std::uninitialized_copy(other.begin(), other.end(), data());
        }

        template<std::size_t OtherN, Requires<OtherN <= max_elements> = true>
        StaticVector(const StaticVector<T, OtherN>& other) :
            m_size(other.m_size)
        {
            std::uninitialized_copy(other.begin(), other.end(), data());
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
                
                clear_region(data() + new_size, data() + m_size);
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
            new(data() + m_size) T(value);
            m_size++;
        }

        void push_back(T&& value)
        {
            assert(m_size < max_elements && "Storage is full");
            new(data() + m_size) T(std::move(value));
            m_size++;
        }

        template<typename... Args>
        void emplace()
        {

        }

        template<typename... Args>
        T& emplace_back(Args&&... args)
        {
            assert(m_size < max_elements && "Storage is full");

            if constexpr (std::is_aggregate_v<T>) {
                ::new(data() + m_size) T{std::forward<Args>(args)...};
            }
            else {
                ::new(data() + m_size) T(std::forward<Args>(args)...);
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
                std::destroy_at(data() + m_size);
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

        Iterator begin() noexcept { return Iterator(data()); }
        ConstIterator begin() const noexcept { return ConstIterator(data()); }
        ConstIterator cbegin() const noexcept { return ConstIterator(data()); }

        Iterator end() noexcept { return data() + m_size; }
        ConstIterator end() const noexcept { return ConstIterator(data() + m_size); }
        ConstIterator cend() const noexcept { return ConstIterator(data() + m_size); }

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