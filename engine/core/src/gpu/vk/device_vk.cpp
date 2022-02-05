#include "device_vk.hpp"
#include "surface_vk.hpp"
#include "resource_vk.hpp"
#include "vk.hpp"

#include <tempeh/common/os.hpp>
#include <map>
#include <GLFW/glfw3.h>

#ifdef TEMPEH_OS_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include <GLFW/glfw3native.h>

namespace Tempeh::GPU
{
    static const char* layers[] = {
        "VK_LAYER_KHRONOS_validation",
        "dummy"
    };

    static const char* instance_extensions[] = {
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
    };

    static const char* device_extensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_MAINTENANCE1_EXTENSION_NAME
    };

    DeviceVK::DeviceVK(
        VkInstance instance,
        VkPhysicalDevice physical_device,
        const VkPhysicalDeviceProperties& properties,
        VkDevice device,
        VmaAllocator allocator,
        u32 main_queue_index)
        : Device(BackendType::Vulkan, "Vulkan"),
          m_instance(instance),
          m_physical_device(physical_device),
          m_properties(properties),
          m_device(device),
          m_allocator(allocator),
          m_main_queue_index(main_queue_index)
    {
        vkGetDeviceQueue(m_device, m_main_queue_index, 0, &m_main_queue);

        m_storage_image_template_descriptors.emplace(m_device);
        m_sampled_image_template_descriptors.emplace(m_device);
        m_sampler_template_descriptors.emplace(m_device);

        m_job_queue = std::make_unique<JobQueueVK>(
            m_device, m_main_queue, m_main_queue_index);
    }

    DeviceVK::~DeviceVK()
    {
        vkDeviceWaitIdle(m_device);
        m_job_queue.reset();
        m_storage_image_template_descriptors.reset();
        m_sampled_image_template_descriptors.reset();
        m_sampler_template_descriptors.reset();
        vmaDestroyAllocator(m_allocator);
        vkDestroyDevice(m_device, nullptr);
        vkDestroyInstance(m_instance, nullptr);
    }

    RefDeviceResult<Surface> DeviceVK::create_surface(
        const std::shared_ptr<Window::Window>& window,
        const SurfaceDesc& desc)
    {
        std::lock_guard lock(m_sync_mutex);
        VkSurfaceKHR vk_surface = VK_NULL_HANDLE;

        if (desc.num_images > 3) {
            return DeviceErrorCode::InvalidArgs;
        }

        switch (window->get_window_type()) {
            case Window::WindowType::GLFW:
                vk_surface = create_surface_glfw(window);
                break;
            default:
                return DeviceErrorCode::Unimplemented;
        }

        if (vk_surface == VK_NULL_HANDLE) {
            return DeviceErrorCode::InternalError;
        }

        VkBool32 present_supported = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(
            m_physical_device, m_main_queue_index,
            vk_surface, &present_supported);

        if (present_supported != VK_TRUE) {
            vkDestroySurfaceKHR(m_instance, vk_surface, nullptr);
            return DeviceErrorCode::SurfacePresentationNotSupported;
        }

        SurfaceVK* surface = new SurfaceVK(vk_surface, this);
        DeviceErrorCode err = surface->initialize(desc);

        if (err != DeviceErrorCode::Ok) {
            delete surface;
            vkDestroySurfaceKHR(m_instance, vk_surface, nullptr);
            return err;
        }

        surface->attach_window(window);

        return Util::Ref<Surface>(surface);
    }

    RefDeviceResult<Texture> DeviceVK::create_texture(const TextureDesc& desc)
    {
        std::lock_guard lock(m_sync_mutex);

        if (desc.width < 1 || desc.height < 1 || desc.depth < 1 ||
            desc.mip_levels < 1 || desc.array_layers < 1 || desc.num_samples < 1)
        {
            return DeviceErrorCode::OutOfRange;
        }

        VkImageCreateInfo image_info{};
        VkImageType image_type = VK_IMAGE_TYPE_1D;
        VkImageViewType view_type = VK_IMAGE_VIEW_TYPE_1D;
        VkImageCreateFlags create_flags = 0;
        VkFormat format = convert_format_vk(desc.format);

        auto [usage, format_feature] = convert_texture_usage_vk(desc.usage);
        auto [format_supported, is_optimal] = is_texture_format_supported(format, format_feature);

        if (!format_supported) {
            return DeviceErrorCode::FormatNotSupported;
        }

        switch (desc.type) {
            case TextureType::Texture1D:
                image_type = VK_IMAGE_TYPE_1D;
                view_type = VK_IMAGE_VIEW_TYPE_1D;
                break;
            case TextureType::TextureArray1D:
                image_type = VK_IMAGE_TYPE_1D;
                view_type = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
                break;
            case TextureType::Texture2D:
                image_type = VK_IMAGE_TYPE_2D;
                view_type = VK_IMAGE_VIEW_TYPE_2D;
                break;
            case TextureType::TextureArray2D:
                image_type = VK_IMAGE_TYPE_2D;
                view_type = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
                break;
            case TextureType::TextureCube:
                image_type = VK_IMAGE_TYPE_2D;
                view_type = VK_IMAGE_VIEW_TYPE_CUBE;
                create_flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
                break;
            case TextureType::TextureArrayCube:
                image_type = VK_IMAGE_TYPE_2D;
                view_type = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
                create_flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
                break;
            case TextureType::Texture3D:
                image_type = VK_IMAGE_TYPE_3D;
                view_type = VK_IMAGE_VIEW_TYPE_3D;
                break;
            default:
                break;
        }

        // Create image
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.flags = create_flags;
        image_info.imageType = image_type;
        image_info.format = format;
        image_info.extent.width = desc.width;
        image_info.extent.height = desc.height;
        image_info.extent.depth = desc.depth;
        image_info.mipLevels = desc.mip_levels;
        image_info.arrayLayers = desc.array_layers;
        image_info.samples = (VkSampleCountFlagBits)desc.num_samples;
        
        image_info.tiling =
            is_optimal ? VK_IMAGE_TILING_OPTIMAL : VK_IMAGE_TILING_LINEAR;

        image_info.usage = usage;
        image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        auto [required_flags, preferred_flags] =
            convert_memory_usage_vk(desc.memory_usage);

        VkImage image;
        VmaAllocation allocation;
        VmaAllocationCreateInfo alloc_info{};

        alloc_info.requiredFlags = required_flags;
        alloc_info.preferredFlags = preferred_flags;

        VkResult result = vmaCreateImage(
            m_allocator, &image_info, &alloc_info,
            &image, &allocation, nullptr);

        if (result == VK_ERROR_FEATURE_NOT_PRESENT) {
            return DeviceErrorCode::MemoryUsageNotSupported;
        }
        else if (VULKAN_FAILED(result)) {
            return DeviceErrorCode::InternalError;
        }

        // Create image view
        VkImageViewCreateInfo view_info{};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = image;
        view_info.viewType = view_type;
        view_info.format = format;
        view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        if (bit_match(desc.usage, TextureUsage::DepthStencilAttachment)) {
            view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        }
        else {
            view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = desc.mip_levels;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = desc.array_layers;

        VkImageView image_view;
        result = vkCreateImageView(m_device, &view_info, nullptr, &image_view);

        if (VULKAN_FAILED(result)) {
            vmaDestroyImage(m_allocator, image, allocation);
            return DeviceErrorCode::InternalError;
        }

        // Prepare template descriptor

        VkDescriptorSet storage_template_descriptor = VK_NULL_HANDLE;

        if (bit_match(desc.usage, TextureUsage::StorageWrite) ||
            bit_match(desc.usage, TextureUsage::StorageRead))
        {
            storage_template_descriptor = m_storage_image_template_descriptors.value().allocate_set();
            
            VkDescriptorImageInfo descriptor_image_info{};
            descriptor_image_info.sampler = VK_NULL_HANDLE;
            descriptor_image_info.imageView = image_view;
            descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

            VkWriteDescriptorSet write_descriptor{};
            write_descriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write_descriptor.dstSet = storage_template_descriptor;
            write_descriptor.dstBinding = 0;
            write_descriptor.dstArrayElement = 0;
            write_descriptor.descriptorCount = 1;
            write_descriptor.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            write_descriptor.pImageInfo = &descriptor_image_info;

            vkUpdateDescriptorSets(m_device, 1, &write_descriptor, 0, nullptr);
        }

        VkDescriptorSet sampled_template_descriptor = VK_NULL_HANDLE;

        if (bit_match(desc.usage, TextureUsage::Sampled)) {
            sampled_template_descriptor = m_sampled_image_template_descriptors.value().allocate_set();
            
            VkDescriptorImageInfo descriptor_image_info{};
            descriptor_image_info.sampler = VK_NULL_HANDLE;
            descriptor_image_info.imageView = image_view;

            VkWriteDescriptorSet write_descriptor{};
            write_descriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write_descriptor.dstSet = sampled_template_descriptor;
            write_descriptor.dstBinding = 0;
            write_descriptor.dstArrayElement = 0;
            write_descriptor.descriptorCount = 1;
            write_descriptor.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            write_descriptor.pImageInfo = &descriptor_image_info;

            if (bit_match(desc.usage, TextureUsage::DepthStencilAttachment)) {
                descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
                vkUpdateDescriptorSets(m_device, 1, &write_descriptor, 0, nullptr);
            }
            else {
                descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                vkUpdateDescriptorSets(m_device, 1, &write_descriptor, 0, nullptr);
            }
        }

        return std::make_shared<TextureVK>(
            this, image, image_view, allocation,
            storage_template_descriptor,
            sampled_template_descriptor, desc);
    }

    RefDeviceResult<Buffer> DeviceVK::create_buffer(const BufferDesc& desc)
    {
        VkBufferCreateInfo buffer_info{};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = desc.size;
        buffer_info.usage = convert_buffer_usage_vk(desc.usage);

        auto [required_flags, preferred_flags] =
            convert_memory_usage_vk(desc.memory_usage);

        VmaAllocationCreateInfo alloc_info{};
        alloc_info.requiredFlags = required_flags;
        alloc_info.preferredFlags = preferred_flags;

        VmaAllocation allocation;
        VkBuffer buffer;
        VkResult result =
            vmaCreateBuffer(m_allocator, &buffer_info,
                &alloc_info, &buffer, &allocation, nullptr);

        if (result == VK_ERROR_FEATURE_NOT_PRESENT) {
            return DeviceErrorCode::MemoryUsageNotSupported;
        }
        else if (VULKAN_FAILED(result)) {
            return DeviceErrorCode::InternalError;
        }

        return std::make_shared<BufferVK>(this, buffer, allocation, desc);
    }

    RefDeviceResult<Framebuffer> DeviceVK::create_framebuffer(const FramebufferDesc& desc)
    {
        TEMPEH_UNREFERENCED(desc);
        return DeviceErrorCode::Unimplemented;
    }

    RefDeviceResult<RenderPass> DeviceVK::create_render_pass(const RenderPassDesc& desc)
    {
        TEMPEH_UNREFERENCED(desc);
        return DeviceErrorCode::Unimplemented;
    }

    void DeviceVK::begin_cmd()
    {
        if (m_is_recording_command) {
            return;
        }

        JobItemVK& job_item = m_job_queue->enqueue_job(); // Create new GPU job

        job_item.wait(); // Wait previous submission on this job item
        job_item.destroy_pending_resources();
        m_current_cmd_buffer = job_item.begin();
        m_is_recording_command = true;
    }

    void DeviceVK::bind_texture(u32 slot, const Util::Ref<Texture>& texture)
    {
        TEMPEH_UNREFERENCED(slot);
        TEMPEH_UNREFERENCED(texture);
    }

    void DeviceVK::end_cmd()
    {
        if (!m_is_recording_command) {
            return;
        }

        vkEndCommandBuffer(m_current_cmd_buffer);
        m_job_queue->dequeue_job(); // submit current job
        m_is_recording_command = false;
    }

    void DeviceVK::wait_idle()
    {
        vkDeviceWaitIdle(m_device);
    }

    std::pair<bool, bool> DeviceVK::is_texture_format_supported(VkFormat format, VkFormatFeatureFlags features) const
    {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(m_physical_device, format, &properties);

        VkFormatFeatureFlags combined_features =
            properties.optimalTilingFeatures | properties.linearTilingFeatures;

        bool supported = bit_match(combined_features, features);
        bool is_optimal = bit_match(properties.optimalTilingFeatures, features);

        return std::make_pair(supported, is_optimal);
    }

    RefDeviceResult<Device> DeviceVK::initialize(bool prefer_high_performance)
    {
        if (VULKAN_FAILED(volkInitialize())) {
            return DeviceErrorCode::InitializationFailed;
        }

        // ---- Vulkan instance initialization ----

        VkApplicationInfo app_info{};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName = "Tempeh Engine";
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName = "Tempeh Engine";
        app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion = VK_API_VERSION_1_0;

        u32 num_layers;
        std::vector<VkLayerProperties> layer_properties;
        std::vector<const char*> enabled_layers;

        vkEnumerateInstanceLayerProperties(&num_layers, nullptr);
        layer_properties.resize(num_layers);
        vkEnumerateInstanceLayerProperties(&num_layers, layer_properties.data());

        for (const auto layer : layers) {
            if (find_layer(layer_properties, layer)) {
                enabled_layers.push_back(layer);
            }
        }

        std::vector<const char*> enabled_exts;
        std::vector<VkExtensionProperties> ext_properties;
        u32 num_extensions;

        vkEnumerateInstanceExtensionProperties(nullptr, &num_extensions, nullptr);
        ext_properties.resize(num_extensions);
        vkEnumerateInstanceExtensionProperties(nullptr, &num_extensions, ext_properties.data());

        for (const auto ext : instance_extensions) {
            if (find_extension(ext_properties, ext)) {
                enabled_exts.push_back(ext);
            }
        }

        VkInstanceCreateInfo instance_info{};
        instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_info.pApplicationInfo = &app_info;
        instance_info.enabledLayerCount = static_cast<uint32_t>(enabled_layers.size());
        instance_info.ppEnabledLayerNames = enabled_layers.data();
        instance_info.enabledExtensionCount = static_cast<uint32_t>(enabled_exts.size());
        instance_info.ppEnabledExtensionNames = enabled_exts.data();

        VkInstance instance = VK_NULL_HANDLE;
        VkResult result = vkCreateInstance(&instance_info, nullptr, &instance);

        if (VULKAN_FAILED(result)) {
            return parse_error_vk(result);
        }

        volkLoadInstance(instance); // Load instance functions

        // ---- Vulkan device initialization ----

        u32 num_physical_devices;
        std::vector<VkPhysicalDevice> physical_devices;

        vkEnumeratePhysicalDevices(instance, &num_physical_devices, nullptr);
        physical_devices.resize(num_physical_devices);
        vkEnumeratePhysicalDevices(instance, &num_physical_devices, physical_devices.data());

        VkPhysicalDevice physical_device = physical_devices[0];
        VkPhysicalDeviceFeatures features{};
        VkPhysicalDeviceProperties properties{};

        if (prefer_high_performance) {
            // Find a discrete GPU
            for (auto current : physical_devices) {
                vkGetPhysicalDeviceProperties(current, &properties);

                if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                    physical_device = current;
                    vkGetPhysicalDeviceFeatures(physical_device, &features);
                    break;
                }
            }
        }
        else {
            vkGetPhysicalDeviceProperties(physical_device, &properties);
            vkGetPhysicalDeviceFeatures(physical_device, &features);
        }

        ext_properties.clear();

        // Query physical device extensions
        vkEnumerateDeviceExtensionProperties(
            physical_device, nullptr, &num_extensions, nullptr);

        ext_properties.resize(num_extensions);
        enabled_exts.clear();

        vkEnumerateDeviceExtensionProperties(
            physical_device, nullptr, &num_extensions, ext_properties.data());

        for (const auto ext : device_extensions) {
            if (find_extension(ext_properties, ext)) {
                enabled_exts.push_back(ext);
            }
        }

        u32 num_queue_families;
        std::vector<VkQueueFamilyProperties> queue_families;

        vkGetPhysicalDeviceQueueFamilyProperties(
            physical_device,
            &num_queue_families,
            nullptr);

        queue_families.resize(num_queue_families);

        vkGetPhysicalDeviceQueueFamilyProperties(
            physical_device,
            &num_queue_families,
            queue_families.data());

        static float queue_priorities = 1.0f;
        VkDeviceQueueCreateInfo queue_info{};
        u32 queue_family_idx = 0;
        bool has_suitable_queue = false;

        // For now, we expect the hardware supports Graphics, Compute and Present on the same queue
        for (const auto& queue_family : queue_families) {
            if (bit_match(queue_family.queueFlags, VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT)) {
                queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queue_info.queueFamilyIndex = queue_family_idx;
                queue_info.queueCount = 1;
                queue_info.pQueuePriorities = &queue_priorities;
                has_suitable_queue = true;
                break;
            }
            queue_family_idx++;
        }

        if (!has_suitable_queue) {
            return DeviceErrorCode::BackendNotSupported;
        }

        VkDeviceCreateInfo device_info{};
        device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_info.queueCreateInfoCount = 1;
        device_info.pQueueCreateInfos = &queue_info;
        device_info.enabledExtensionCount = static_cast<uint32_t>(enabled_exts.size());
        device_info.ppEnabledExtensionNames = enabled_exts.data();
        device_info.pEnabledFeatures = &features;

        VkDevice device = VK_NULL_HANDLE;
        result = vkCreateDevice(physical_device, &device_info, nullptr, &device);

        if (VULKAN_FAILED(result)) {
            vkDestroyInstance(instance, nullptr);
            return parse_error_vk(result);
        }

        volkLoadDevice(device);

        VmaVulkanFunctions vma_fn{};
        vma_fn.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
        vma_fn.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
        vma_fn.vkAllocateMemory = vkAllocateMemory;
        vma_fn.vkFreeMemory = vkFreeMemory;
        vma_fn.vkMapMemory = vkMapMemory;
        vma_fn.vkUnmapMemory = vkUnmapMemory;
        vma_fn.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
        vma_fn.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
        vma_fn.vkBindBufferMemory = vkBindBufferMemory;
        vma_fn.vkBindImageMemory = vkBindImageMemory;
        vma_fn.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
        vma_fn.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
        vma_fn.vkCreateBuffer = vkCreateBuffer;
        vma_fn.vkDestroyBuffer = vkDestroyBuffer;
        vma_fn.vkCreateImage = vkCreateImage;
        vma_fn.vkDestroyImage = vkDestroyImage;
        vma_fn.vkCmdCopyBuffer = vkCmdCopyBuffer;

        VmaAllocatorCreateInfo allocator_info{};
        allocator_info.vulkanApiVersion = app_info.apiVersion;
        allocator_info.physicalDevice = physical_device;
        allocator_info.device = device;
        allocator_info.instance = instance;
        allocator_info.pVulkanFunctions = &vma_fn;

        VmaAllocator allocator;
        vmaCreateAllocator(&allocator_info, &allocator);

        return std::static_pointer_cast<Device>(
            std::make_shared<DeviceVK>(
                instance, physical_device, properties,
                device, allocator, queue_info.queueFamilyIndex));
    }

    VkSurfaceKHR DeviceVK::create_surface_glfw(const std::shared_ptr<Window::Window>& window)
    {
        VkSurfaceKHR ret = VK_NULL_HANDLE;

        glfwCreateWindowSurface(
            m_instance,
            static_cast<GLFWwindow*>(window->get_raw_handle()),
            nullptr,
            &ret);

        return ret;
    }

    JobItemVK::JobItemVK(VkDevice vk_device, u32 queue_family_index) :
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
    
    JobItemVK::~JobItemVK()
    {
    }

    void JobItemVK::wait()
    {
        vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
        vkResetFences(device, 1, &fence);
    }

    VkCommandBuffer JobItemVK::begin()
    {
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkResetCommandPool(device, cmd_pool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);

        VULKAN_ASSERT(!VULKAN_FAILED(
            vkBeginCommandBuffer(cmd_buffer, &begin_info)));

        return cmd_buffer;
    }

    void JobItemVK::destroy_cmd_buffer()
    {
        // Will also destroy command buffers
        vkDestroyCommandPool(device, cmd_pool, nullptr);
        vkDestroyFence(device, fence, nullptr);
    }

    void JobItemVK::destroy_pending_resources()
    {
        
    }

    void JobQueueVK::dequeue_job()
    {
        VkSubmitInfo submission{};
        JobItemVK& job_item = job_list[read_pointer];

        submission.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submission.commandBufferCount = 1;
        submission.pCommandBuffers = &job_item.cmd_buffer;

        VULKAN_ASSERT(!VULKAN_FAILED(
            vkQueueSubmit(cmd_queue, 1, &submission, job_item.fence)));

        read_pointer = (read_pointer + 1) % max_job;
    }
}
