#ifndef _TEMPEH_GPU_DETAIL_COMMAND_LIST_IMPL_H
#define _TEMPEH_GPU_DETAIL_COMMAND_LIST_IMPL_H

namespace Tempeh::GPU::Detail
{
    struct CommandListImpl
    {
        CommandListImpl();
        virtual ~CommandListImpl();

        virtual void begin() = 0;
        virtual void end() = 0;
    };
}

#endif