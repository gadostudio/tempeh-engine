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
            var_(std::move(value))
        {
        }

        constexpr Result(E&& err) noexcept :
            var_(std::move(err))
        {
        }

        constexpr Result(const Result& other) :
            var_(other.var_)
        {
        }

        constexpr Result(Result&& other) noexcept :
            var_(std::move(other.var_))
        {
        }

        Result& operator=(T&& value)
        {
            var_ = std::move(value);
            return *this;
        }

        Result& operator=(E&& err)
        {
            var_ = std::move(err);
            return *this;
        }

        Result& operator=(const Result& other)
        {
            var_ = other.var_;
            return *this;
        }

        Result& operator=(Result&& other)
        {
            var_ = std::move(other.var_);
            return *this;
        }

        bool is_ok() const
        {
            return std::holds_alternative<T>(var_);
        }

        T& value()
        {
            return std::get<T>(var_);
        }

        const T& value() const
        {
            return std::get<T>(var_);
        }

        const E& err() const
        {
            return std::get<E>(var_);
        }

    private:
        std::variant<T, E> var_;
    };
}

#endif