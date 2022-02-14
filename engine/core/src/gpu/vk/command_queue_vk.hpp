#ifndef _TEMPEH_GPU_COMMAND_QUEUE_VK_HPP
#define _TEMPEH_GPU_COMMAND_QUEUE_VK_HPP 1

#include <deque>
#include <utility>

#include "vk.hpp"

namespace Tempeh::GPU
{
    struct CommandSubmissionVK
    {
        std::deque<std::tuple<VkImage, VkImageView, VmaAllocation>> texture_free_queue;
        std::deque<std::tuple<VkBuffer, VmaAllocation>> buffer_free_queue;

        VkDevice device = VK_NULL_HANDLE;
        VkCommandPool cmd_pool = VK_NULL_HANDLE;
        VkCommandBuffer cmd_buffer = VK_NULL_HANDLE;
        VkSemaphore semaphore = VK_NULL_HANDLE;
        VkFence fence = VK_NULL_HANDLE;

        CommandSubmissionVK(VkDevice vk_device, u32 queue_family_index);
        ~CommandSubmissionVK();

        void wait();
        VkCommandBuffer begin_cmd_buffer();
        void destroy_cmd_buffers();
        void destroy_pending_resources();
    };

    struct CommandQueueVK
    {
        static constexpr u32 max_submissions = 4;

        VkDevice device;
        VkQueue cmd_queue;
        u32 queue_family_index;
        std::vector<CommandSubmissionVK> submission_list;
        std::size_t read_pointer = 0;
        std::size_t write_pointer = 0;

        CommandQueueVK(VkDevice vk_device, VkQueue cmd_queue, u32 queue_family_index);
        ~CommandQueueVK();

        std::tuple<CommandSubmissionVK*, std::size_t> enqueue_submission();
        void dequeue_submission();
    };
}

#endif