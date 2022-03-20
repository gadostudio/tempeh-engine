#ifndef _TEMPEH_GPU_COMMAND_QUEUE_VK_HPP
#define _TEMPEH_GPU_COMMAND_QUEUE_VK_HPP 1

#include <queue>
#include <utility>

#include "vk.hpp"

namespace Tempeh::GPU
{
    struct CommandSubmissionVK
    {
        std::queue<std::tuple<VkImage, VkImageView, VmaAllocation>> texture_free_queue;
        std::queue<std::tuple<VkBuffer, VmaAllocation>> buffer_free_queue;
        std::queue<VkRenderPass> render_pass_free_queue;
        std::queue<VkFramebuffer> framebuffer_free_queue;
        std::queue<VkSampler> sampler_free_queue;
        std::queue<VkPipelineLayout> pipeline_layout_free_queue;
        std::queue<VkPipeline> graphics_pipeline_free_queue;

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
        void destroy_pending_resources(VmaAllocator allocator);
    };

    struct CommandQueueVK
    {
        static constexpr u32 max_submissions = 4;

        VkDevice device;
        VkQueue cmd_queue;
        VmaAllocator allocator;
        u32 queue_family_index;
        std::vector<CommandSubmissionVK> submission_list;
        std::size_t read_pointer = 0;
        std::size_t write_pointer = 0;

        CommandQueueVK(VkDevice vk_device, VkQueue cmd_queue, VmaAllocator allocator, u32 queue_family_index);
        ~CommandQueueVK();

        std::tuple<CommandSubmissionVK*, std::size_t> enqueue_submission();
        void dequeue_submission();

        void destroy_texture(
            std::size_t submission_id,
            VkImage image,
            VkImageView image_view,
            VmaAllocation allocation);

        void destroy_buffer(
            std::size_t submission_id,
            VkBuffer buffer,
            VmaAllocation allocation);
        
        void destroy_render_pass(std::size_t submission_id, VkRenderPass render_pass);
        void destroy_framebuffer(std::size_t submission_id, VkFramebuffer framebuffer);
        void destroy_sampler(std::size_t submission_id, VkSampler sampler);
        void destroy_pipeline_layout(std::size_t submission_id, VkPipelineLayout layout);
        void destroy_graphics_pipeline(std::size_t submission_id, VkPipeline graphics_pipeline);
    };
}

#endif