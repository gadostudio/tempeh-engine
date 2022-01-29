#ifndef _TEMPEH_UTIL_RESULT_H
#define _TEMPEH_UTIL_RESULT_H

#include <variant>

namespace Tempeh::Util
{
    template<typename T, typename E, std::enable_if_t<!std::is_same_v<T, E>, bool> = true>
    class Result
    {
    public:
        constexpr Result()
        {
        }

        constexpr Result(T&& value) noexcept :
            m_var(std::move(value))
        {
        }

        constexpr Result(E&& err) noexcept :
            m_var(std::move(err))
        {
        }

        constexpr Result(const Result& other) :
            m_var(other.m_var)
        {
        }

        constexpr Result(Result&& other) noexcept :
            m_var(std::move(other.m_var))
        {
        }

        Result& operator=(T&& value)
        {
            m_var = std::move(value);
            return *this;
        }

        Result& operator=(E&& err)
        {
            m_var = std::move(err);
            return *this;
        }

        Result& operator=(const Result& other)
        {
            m_var = other.m_var;
            return *this;
        }

        Result& operator=(Result&& other)
        {
            m_var = std::move(other.m_var);
            return *this;
        }

        bool is_ok() const
        {
            return std::holds_alternative<T>(m_var);
        }

        T& value()
        {
            return std::get<T>(m_var);
        }

        const T& value() const
        {
            return std::get<T>(m_var);
        }

        const E& err() const
        {
            return std::get<E>(m_var);
        }

    private:
        std::variant<T, E> m_var;
    };
}

#endif