#ifndef _TEMPEH_GPU_DETAIL_COMMAND_LIST_IMPL_H
#define _TEMPEH_GPU_DETAIL_COMMAND_LIST_IMPL_H

namespace Tempeh::GPU::Detail
{
    struct CommandListImpl
    {
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