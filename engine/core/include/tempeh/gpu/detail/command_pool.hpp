#ifndef _TEMPEH_GPU_DETAIL_COMMAND_POOL_HPP
#define _TEMPEH_GPU_DETAIL_COMMAND_POOL_HPP

#include <tempeh/common/typedefs.hpp>

namespace Tempeh::GPU
{
    class CommandPool
    {
    public:
        CommandPool(size_t block_size) :
            m_block_size(block_size),
            m_whole_block_size(block_size + sizeof(u8*)), // block size + node pointer
            m_head_block(nullptr),
            m_current_block(nullptr)
        {
            reserve_block();
        }

        ~CommandPool()
        {
        }

        template<typename T, typename... Args>
        T* allocate_command(Args&&... args)
        {
            return nullptr;
        }

    private:
        size_t m_block_size;
        size_t m_whole_block_size;
        u8* m_head_block;
        u8* m_current_block;

        void reserve_block()
        {
            if (m_head_block == nullptr) {
                u8* block = new u8[m_whole_block_size];
                *reinterpret_cast<u8**>(block) = nullptr;
            }
        }
    };
}

#endif