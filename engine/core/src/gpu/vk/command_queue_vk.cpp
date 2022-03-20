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
        vkDestroyCommandPool(device, cmd_pool, nullptr); // Will also destroy command buffers
        vkDestroyFence(device, fence, nullptr);
    }

    void CommandSubmissionVK::destroy_pending_resources(VmaAllocator allocator)
    {
        while (!texture_free_queue.empty()) {
            auto& [image, image_view, allocation] = texture_free_queue.front();
            vkDestroyImageView(device, image_view, nullptr);
            vmaDestroyImage(allocator, image, allocation);
            texture_free_queue.pop();
        }

        while (!buffer_free_queue.empty()) {
            auto& [buffer, allocation] = buffer_free_queue.front();
            vmaDestroyBuffer(allocator, buffer, allocation);
            buffer_free_queue.pop();
        }

        while (!render_pass_free_queue.empty()) {
            VkRenderPass render_pass = render_pass_free_queue.front();
            vkDestroyRenderPass(device, render_pass, nullptr);
            render_pass_free_queue.pop();
        }

        while (!framebuffer_free_queue.empty()) {
            VkFramebuffer framebuffer = framebuffer_free_queue.front();
            vkDestroyFramebuffer(device, framebuffer, nullptr);
            framebuffer_free_queue.pop();
        }

        while (!sampler_free_queue.empty()) {
            VkSampler sampler = sampler_free_queue.front();
            vkDestroySampler(device, sampler, nullptr);
            sampler_free_queue.pop();
        }

        while (!pipeline_layout_free_queue.empty()) {
            VkPipelineLayout layout = pipeline_layout_free_queue.front();
            vkDestroyPipelineLayout(device, layout, nullptr);
            pipeline_layout_free_queue.pop();
        }

        while (!graphics_pipeline_free_queue.empty()) {
            VkPipeline pipeline = graphics_pipeline_free_queue.front();
            vkDestroyPipeline(device, pipeline, nullptr);
            graphics_pipeline_free_queue.pop();
        }
    }

    CommandQueueVK::CommandQueueVK(
        VkDevice vk_device,
        VkQueue cmd_queue,
        VmaAllocator allocator,
        u32 queue_family_index)
        : device(vk_device),
          cmd_queue(cmd_queue),
          allocator(allocator),
          queue_family_index(queue_family_index)
    {
        submission_list.reserve(max_submissions);
    }

    CommandQueueVK::~CommandQueueVK()
    {
        for (auto& job : submission_list) {
            job.destroy_cmd_buffers();
            job.destroy_pending_resources(allocator);
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

        return std::make_tuple(&submission_list[current_job], current_job);
    }

    void CommandQueueVK::dequeue_submission()
    {
        VkSubmitInfo submit_info{};
        CommandSubmissionVK& job_item = submission_list[read_pointer];

        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &job_item.cmd_buffer;

        VULKAN_ASSERT(!VULKAN_FAILED(vkQueueSubmit(cmd_queue, 1, &submit_info, job_item.fence)));

        read_pointer = (read_pointer + 1) % max_submissions;
    }

    void CommandQueueVK::destroy_texture(std::size_t submission_id,
                                         VkImage image,
                                         VkImageView image_view,
                                         VmaAllocation allocation)
    {
        // VULKAN_INVALID_QUEUE_SUBMISSION means the resource is not currently used by any submission.
        if (submission_id == VULKAN_INVALID_QUEUE_SUBMISSION) {
            vkDestroyImageView(device, image_view, nullptr);
            vmaDestroyImage(allocator, image, allocation);
            return;
        }

        submission_list[submission_id].texture_free_queue.emplace(image, image_view, allocation);
    }

    void CommandQueueVK::destroy_buffer(std::size_t submission_id, VkBuffer buffer, VmaAllocation allocation)
    {
        if (submission_id == VULKAN_INVALID_QUEUE_SUBMISSION) {
            vmaDestroyBuffer(allocator, buffer, allocation);
            return;
        }

        submission_list[submission_id].buffer_free_queue.emplace(buffer, allocation);
    }

    void CommandQueueVK::destroy_render_pass(std::size_t submission_id, VkRenderPass render_pass)
    {
        if (submission_id == VULKAN_INVALID_QUEUE_SUBMISSION) {
            vkDestroyRenderPass(device, render_pass, nullptr);
            return;
        }

        submission_list[submission_id].render_pass_free_queue.emplace(render_pass);
    }

    void CommandQueueVK::destroy_framebuffer(std::size_t submission_id, VkFramebuffer framebuffer)
    {
        if (submission_id == VULKAN_INVALID_QUEUE_SUBMISSION) {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
            return;
        }

        submission_list[submission_id].framebuffer_free_queue.emplace(framebuffer);
    }

    void CommandQueueVK::destroy_sampler(std::size_t submission_id, VkSampler sampler)
    {
        if (submission_id == VULKAN_INVALID_QUEUE_SUBMISSION) {
            vkDestroySampler(device, sampler, nullptr);
            return;
        }

        submission_list[submission_id].sampler_free_queue.emplace(sampler);
    }

    void CommandQueueVK::destroy_pipeline_layout(std::size_t submission_id, VkPipelineLayout layout)
    {
        if (submission_id == VULKAN_INVALID_QUEUE_SUBMISSION) {
            vkDestroyPipelineLayout(device, layout, nullptr);
            return;
        }

        submission_list[submission_id].pipeline_layout_free_queue.emplace(layout);
    }
    
    void CommandQueueVK::destroy_graphics_pipeline(std::size_t submission_id, VkPipeline graphics_pipeline)
    {
        if (submission_id == VULKAN_INVALID_QUEUE_SUBMISSION) {
            vkDestroyPipeline(device, graphics_pipeline, nullptr);
            return;
        }

        submission_list[submission_id]
            .graphics_pipeline_free_queue
            .emplace(graphics_pipeline);
    }
}
