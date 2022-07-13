#ifndef TEMPEH_ENGINE_ENGINE_COMMON_INCLUDE_TEMPEH_COMMON_THREAD_HPP
#define TEMPEH_ENGINE_ENGINE_COMMON_INCLUDE_TEMPEH_COMMON_THREAD_HPP

#include <windows.h>

inline void name_thread(char* name)
{
#if defined(TEMPEH_OS_WINDOWS)
    SetThreadDescription(io_thread.native_handle(), L"IO");
#endif
}

#endif //TEMPEH_ENGINE_ENGINE_COMMON_INCLUDE_TEMPEH_COMMON_THREAD_HPP
