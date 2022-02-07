#ifndef _TEMPEH_GPU_DEVICE_VK_H
#define _TEMPEH_GPU_DEVICE_VK_H

#include <tempeh/gpu/device.hpp>
#include <memory>
#include <array>
#include <deque>
#include <mutex>
#include <optional>

#include "backend_vk.hpp"
#include "vk.hpp"
#include "template_descriptors_vk.hpp"
#include "command_state_vk.hpp"

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
        size_t read_pointer = 0;
        size_t write_pointer = 0;

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
            size_t current_job = write_pointer;
            size_t job_incr = write_pointer + 1;

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
        static constexpr u32 max_resources = 2048;
        using StorageImageTemplateDescriptors = TemplateDescriptorsVK<VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, max_resources>;
        using SampledImageTemplateDescriptors = TemplateDescriptorsVK<VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, max_resources>;
        using SamplerTemplateDescriptors = TemplateDescriptorsVK<VK_DESCRIPTOR_TYPE_SAMPLER, max_resources>;

        VkInstance m_instance;
        VkPhysicalDevice m_physical_device;
        VkPhysicalDeviceProperties m_properties;
        VkDevice m_device;
        VmaAllocator m_allocator;
        VkQueue m_main_queue = VK_NULL_HANDLE;
        u32 m_main_queue_index;
        DeviceLimits m_device_limits{};

        std::mutex m_sync_mutex;
        std::vector<VkSurfaceFormatKHR> m_surface_formats;
        std::vector<VkPresentModeKHR> m_present_modes;
        std::unordered_map<VkFormat, VkFormatProperties> m_format_properties;

        // Template descriptors for copying descriptors
        std::optional<StorageImageTemplateDescriptors> m_storage_image_template_descriptors;
        std::optional<SampledImageTemplateDescriptors> m_sampled_image_template_descriptors;
        std::optional<SamplerTemplateDescriptors> m_sampler_template_descriptors;

        std::unique_ptr<JobQueueVK> m_job_queue;
        VkCommandBuffer m_current_cmd_buffer = VK_NULL_HANDLE;
        CommandStateVK m_cmd_states;
        bool m_is_recording_command = false;
        bool m_is_inside_render_pass = false;

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
        RefDeviceResult<RenderPass> create_render_pass(const RenderPassDesc& desc) override final;
        RefDeviceResult<Framebuffer> create_framebuffer(const Util::Ref<RenderPass>& render_pass, const FramebufferDesc& desc) override final;
        RefDeviceResult<Sampler> create_sampler(const SamplerDesc& desc) override final;

        void begin_cmd() override final;
        void bind_texture(u32 slot, const Util::Ref<Texture>& texture) override final;
        
        void begin_render_pass(
            const Util::Ref<RenderPass>& render_pass,
            const Util::Ref<Framebuffer>& framebuffer,
            std::initializer_list<ClearValue> clear_values,
            ClearValue depth_stencil_clear_value = {}) override final;

        void set_viewport(float x, float y, float width, float height, float min_depth, float max_depth) override final;
        void set_scissor_rect(u32 x, u32 y, u32 width, u32 height) override final;
        void set_blend_constants(float r, float g, float b, float a) override final;
        void set_blend_constants(float color[4]) override final;
        void set_stencil_ref(u32 reference) override final;

        void end_render_pass() override final;
        
        void end_cmd() override final;

        std::pair<bool, bool> is_texture_format_supported(VkFormat format, VkFormatFeatureFlags features) const;

        void wait_idle();

        static RefDeviceResult<Device> initialize(bool prefer_high_performance);

    private:
        VkSurfaceKHR create_surface_glfw(const std::shared_ptr<Window::Window>& window);
    };
}

#endif