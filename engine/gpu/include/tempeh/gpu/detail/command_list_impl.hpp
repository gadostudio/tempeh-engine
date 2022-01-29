#ifndef _TEMPEH_GPU_DETAIL_COMMAND_LIST_IMPL_H
#define _TEMPEH_GPU_DETAIL_COMMAND_LIST_IMPL_H

#include "command_pool.hpp"
#include <tempeh/gpu/types.hpp>

namespace Tempeh::GPU::Detail
{
    struct CommandListImpl
    {
        CommandPool pool;

        CommandListImpl() { }
        virtual ~CommandListImpl() { }

        virtual void begin() = 0;
        virtual void resolve_commands() = 0;

        template<typename Cmd, typename... Args>
        void write_command(Args&&... args)
        {
            // TODO
        }
    };
}

#endif