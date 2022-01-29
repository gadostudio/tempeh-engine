#ifndef _TEMPEH_GPU_COMMANDS_H
#define _TEMPEH_GPU_COMMANDS_H

namespace Tempeh::GPU
{
    enum class CommandType
    {
        Begin,

        BeginRenderPass,
        BindGraphicsPipeline,
        BindIndexBuffer,
        BindVertexBuffers,
        SetViewports,
        SetScissorRects,
        SetBlendConstants,
        SetStencilRef,
        Draw,
        DrawIndexed,
        DrawInstanced,
        DrawIndexedInstanced,
        EndRenderPass,

        BeginComputePass,
        BindComputePipeline,
        Dispatch,
        EndComputePass,

        End
    };

    struct CommandBase
    {
        CommandType type;
        size_t      size;
    };

    template<typename Cmd, CommandType Type>
    struct Command : public CommandBase
    {
        Command() : CommandBase{ sizeof(Cmd), Type }
        {
        }
    };
    
    struct CmdBegin :
        public Command<CmdBegin, CommandType::Begin>
    {
    };

    struct CmdBeginRenderPass :
        public Command<CmdBeginRenderPass, CommandType::BeginRenderPass>
    {
    };

    struct CmdBindGraphicsPipeline :
        public Command<CmdBindGraphicsPipeline, CommandType::BindGraphicsPipeline>
    {
    };

    struct CmdBindIndexBuffer :
        public Command<CmdBegin, CommandType::BindIndexBuffer>
    {
    };

    struct CmdBindVertexBuffers :
        public Command<CmdBegin, CommandType::BindVertexBuffers>
    {
    };

    struct CmdDraw :
        public Command<CmdDraw, CommandType::Draw>
    {
    };

    struct CmdDrawIndexed :
        public Command<CmdDrawIndexed, CommandType::DrawIndexed>
    {
    };

    struct CmdEnd :
        public Command<CmdEnd, CommandType::End>
    {
    };
}

#endif