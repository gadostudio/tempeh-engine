#ifndef _TEMPEH_GPU_DEVICE_VK_H
#define _TEMPEH_GPU_DEVICE_VK_H

#include <tempeh/gpu/device.hpp>
#include <memory>
#include <array>
#include <deque>
#include <mutex>

#include "backend_vk.hpp"
#include "vk.hpp"

namespace Tempeh::GPU
{
    struct JobItemVK
    {
        std::deque<std::tuple<VkImage, VkImageView, VmaAllocation>> texture_free_queue;
        std::deque<std::tuple<VkBuffer, VmaAllocation>> buffer_free_queue;

        VkDevice device = VK_NULL_HANDLE;
        VkCommandPool cmd_pool = VK_NULL_HANDLE;
        VkCommandBuffer cmd_buffer = VK_NULL_HANDLE;
        VkSemaphore semaphore = VK_NULL_HANDLE;
        VkFence fence = VK_NULL_HANDLE;

        JobItemVK(VkDevice vk_device, u32 queue_family_index);
        ~JobItemVK();

        void wait();
        VkCommandBuffer begin();
        void destroy_cmd_buffer();
        void destroy_pending_resources();
    };

    // Class for managing GPU jobs
    struct JobQueueVK
    {
        static constexpr u32 max_job = 4;

        VkDevice device;
        VkQueue cmd_queue;
        u32 queue_family_index;
        std::vector<JobItemVK> job_list;
        u32 read_pointer = 0;
        u32 write_pointer = 0;

        JobQueueVK(VkDevice vk_device, VkQueue cmd_queue, u32 queue_family_index) :
            device(vk_device),
            cmd_queue(cmd_queue),
            queue_family_index(queue_family_index)
        {
            job_list.reserve(max_job);
        }

        ~JobQueueVK()
        {
            for (auto& job : job_list) {
                job.destroy_cmd_buffer();
                job.destroy_pending_resources();
            }
        }

        JobItemVK& enqueue_job()
        {
            u32 current_job = write_pointer;
            u32 job_incr = write_pointer + 1;

            if (job_incr > job_list.size() && job_list.size() < max_job) {
                job_list.emplace_back(device, queue_family_index);
            }

            write_pointer = job_incr % max_job;

            return job_list[current_job];
        }

        void dequeue_job();
    };

    struct DeviceVK : public Device
    {
        static constexpr size_t max_cmd_buffers = 16;

        VkInstance m_instance;
        VkPhysicalDevice m_physical_device;
        VkPhysicalDeviceProperties m_properties;
        VkDevice m_device;
        VmaAllocator m_allocator;
        VkQueue m_main_queue = VK_NULL_HANDLE;
        u32 m_main_queue_index;

        std::mutex m_sync_mutex;
        std::vector<VkSurfaceFormatKHR> m_surface_formats;
        std::vector<VkPresentModeKHR> m_present_modes;

        std::unique_ptr<JobQueueVK> m_job_queue;
        VkCommandBuffer m_current_cmd_buffer;

        DeviceVK(
            VkInstance instance,
            VkPhysicalDevice physical_device,
            const VkPhysicalDeviceProperties& properties,
            VkDevice device,
            VmaAllocator allocator,
            u32 main_queue_index);

        ~DeviceVK();

        RefDeviceResult<Surface> create_surface(
            const std::shared_ptr<Window::Window>& window,
            const SurfaceDesc& desc) override final;

        RefDeviceResult<Texture> create_texture(const TextureDesc& desc) override final;
        RefDeviceResult<Buffer> create_buffer(const BufferDesc& desc) override final;

        void begin_cmd() override final;
        void end_cmd() override final;

        std::pair<bool, bool> is_format_supported_for_texture(VkFormat format, VkFormatFeatureFlags features) const;
        bool is_format_supported_for_buffer(VkFormat format) const;

        void wait_idle();

        static RefDeviceResult<Device> initialize(bool prefer_high_performance);

    private:
        VkSurfaceKHR create_surface_glfw(const std::shared_ptr<Window::Window>& window);
    };
}

#endif