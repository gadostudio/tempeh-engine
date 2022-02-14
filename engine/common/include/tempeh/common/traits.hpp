#ifndef _TEMPEH_UTIL_TRAITS_HPP
#define _TEMPEH_UTIL_TRAITS_HPP

#include <type_traits>

namespace Tempeh
{
    template<bool Condition>
    using Requires = std::enable_if_t<Condition, bool>;

    template<typename T>
    using RequiresDefaultConstructible = Requires<std::is_default_constructible_v<T>>;

    template<typename T>
    using RequiresCopyConstructible = Requires<std::is_copy_constructible_v<T>>;

    template<typename T>
    using RequiresMoveConstructible = Requires<std::is_move_constructible_v<T>>;
}

#endif