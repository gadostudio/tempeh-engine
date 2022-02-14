#include "command_queue_vk.hpp"

namespace Tempeh::GPU
{
    CommandSubmissionVK::CommandSubmissionVK(VkDevice vk_device, u32 queue_family_index) :
        device(vk_device)
    {
        VkCommandPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        pool_info.queueFamilyIndex = queue_family_index;

        VULKAN_ASSERT(!VULKAN_FAILED(
            vkCreateCommandPool(vk_device, &pool_info, nullptr, &cmd_pool)) &&
            "Failed to create job command pool");

        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool = cmd_pool;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = 1;

        VULKAN_ASSERT(!VULKAN_FAILED(
            vkAllocateCommandBuffers(vk_device, &alloc_info, &cmd_buffer)) &&
            "Failed to create job command buffer");

        /*
        VkSemaphoreCreateInfo semaphore_info{};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VULKAN_ASSERT(!VULKAN_FAILED(
            vkCreateSemaphore(device, &semaphore_info, nullptr, &semaphore)) &&
            "Failed to create job semaphore");
        */

        VkFenceCreateInfo fence_info{};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VULKAN_ASSERT(!VULKAN_FAILED(
            vkCreateFence(device, &fence_info, nullptr, &fence)) &&
            "Failed to create job fence");
    }

    CommandSubmissionVK::~CommandSubmissionVK()
    {
    }

    void CommandSubmissionVK::wait()
    {
        vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
        vkResetFences(device, 1, &fence);
    }

    VkCommandBuffer CommandSubmissionVK::begin_cmd_buffer()
    {
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkResetCommandPool(device, cmd_pool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);

        VULKAN_ASSERT(!VULKAN_FAILED(
            vkBeginCommandBuffer(cmd_buffer, &begin_info)));

        return cmd_buffer;
    }

    void CommandSubmissionVK::destroy_cmd_buffers()
    {
        // Will also destroy command buffers
        vkDestroyCommandPool(device, cmd_pool, nullptr);
        vkDestroyFence(device, fence, nullptr);
    }

    void CommandSubmissionVK::destroy_pending_resources()
    {

    }

    CommandQueueVK::CommandQueueVK(VkDevice vk_device, VkQueue cmd_queue, u32 queue_family_index) :
        device(vk_device),
        cmd_queue(cmd_queue),
        queue_family_index(queue_family_index)
    {
        submission_list.reserve(max_submissions);
    }

    CommandQueueVK::~CommandQueueVK()
    {
        for (auto& job : submission_list) {
            job.destroy_cmd_buffers();
            job.destroy_pending_resources();
        }
    }

    std::tuple<CommandSubmissionVK*, std::size_t> CommandQueueVK::enqueue_submission()
    {
        std::size_t current_job = write_pointer;
        std::size_t job_incr = write_pointer + 1;

        if (job_incr > submission_list.size() && submission_list.size() < max_submissions) {
            submission_list.emplace_back(device, queue_family_index);
        }

        write_pointer = job_incr % max_submissions;

        return std::tuple(&submission_list[current_job], current_job);
    }

    void CommandQueueVK::dequeue_submission()
    {
        VkSubmitInfo submit_info{};
        CommandSubmissionVK& job_item = submission_list[read_pointer];

        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &job_item.cmd_buffer;

        VULKAN_ASSERT(!VULKAN_FAILED(
            vkQueueSubmit(cmd_queue, 1, &submit_info, job_item.fence)));

        read_pointer = (read_pointer + 1) % max_submissions;
    }
}
