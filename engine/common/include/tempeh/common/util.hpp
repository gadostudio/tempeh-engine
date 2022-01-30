#ifndef _TEMPEH_UTIL_HPP
#define _TEMPEH_UTIL_HPP

#include <string_view>

template <typename T = u8>
static inline constexpr T bit(T n)
{
    return 1 << n;
};

struct Lambda {
    template<typename Tret, typename T>
    static Tret lambda_ptr_exec(void* data) {
        return (Tret)(*(T*)fn<T>())(data);
    }

    template<typename Tret = void, typename Tfp = Tret(*)(void*), typename T>
    static Tfp ptr(T& t) {
        fn<T>(&t);
        return (Tfp)lambda_ptr_exec<Tret, T>;
    }

    template<typename T>
    static void* fn(void* new_fn = nullptr) {
        static void* fn;
        if (new_fn != nullptr)
            fn = new_fn;
        return fn;
    }
};

#endif // _TEMPEH_UTIL_HPP
