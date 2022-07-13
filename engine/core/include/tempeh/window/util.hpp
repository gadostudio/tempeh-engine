#ifndef TEMPEH_ENGINE_ENGINE_CORE_INCLUDE_TEMPEH_WINDOW_UTIL_HPP
#define TEMPEH_ENGINE_ENGINE_CORE_INCLUDE_TEMPEH_WINDOW_UTIL_HPP

#include <tempeh/common/typedefs.hpp>

namespace Tempeh::Window
{
    struct WindowSize
    {
        u32 width;
        u32 height;
    };

    struct WindowPosition
    {
        // Signed integer, to support dual-monitor setup
        i32 x, y;
    };
}

#endif //TEMPEH_ENGINE_ENGINE_CORE_INCLUDE_TEMPEH_WINDOW_UTIL_HPP
