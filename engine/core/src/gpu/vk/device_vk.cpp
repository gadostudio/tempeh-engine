#include "device_vk.hpp"
#include "surface_vk.hpp"
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
        "VK_LAYER_KHRONOS_validation"
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
        VkDevice device,
        VmaAllocator allocator,
        u32 main_queue_index)
        : Device(BackendType::Vulkan, "Vulkan"),
          m_instance(instance),
          m_physical_device(physical_device),
          m_device(device),
          m_allocator(allocator),
          m_main_queue_index(main_queue_index)
    {
        vkGetDeviceQueue(m_device, m_main_queue_index, 0, &m_main_queue);

        m_cmd_manager = std::make_unique<CommandManagerVK<max_command_buffers>>(
            m_device, m_main_queue_index);
    }

    DeviceVK::~DeviceVK()
    {
        vkDeviceWaitIdle(m_device);
        m_cmd_manager.reset();
        vmaDestroyAllocator(m_allocator);
        vkDestroyDevice(m_device, nullptr);
        vkDestroyInstance(m_instance, nullptr);
    }

    RefDeviceResult<Surface> DeviceVK::create_surface(
        const std::shared_ptr<Window::Window>& window,
        const SurfaceDesc& desc)
    {
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
        VkImageCreateInfo image_info;
        VkImageType image_type;

        switch (desc.type) {
            case TextureType::Texture1D:
            case TextureType::TextureArray1D:
                image_type = VK_IMAGE_TYPE_1D;
                break;
            case TextureType::Texture2D:
            case TextureType::TextureArray2D:
                image_type = VK_IMAGE_TYPE_2D;
                break;
            case TextureType::Texture3D:
                image_type = VK_IMAGE_TYPE_3D;
                break;
        }

        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType = image_type;
        image_info.format;
        image_info.extent;
        image_info.mipLevels;
        image_info.arrayLayers;
        image_info.samples;
        image_info.tiling;
        image_info.usage;
        image_info.sharingMode;
        image_info.queueFamilyIndexCount;
        image_info.pQueueFamilyIndices;
        image_info.initialLayout;


        return DeviceErrorCode::Unimplemented;
    }

    RefDeviceResult<Buffer> DeviceVK::create_buffer(const BufferDesc& desc)
    {
        return DeviceErrorCode::Unimplemented;
    }

    void DeviceVK::begin_frame()
    {
    }

    void DeviceVK::end_frame()
    {
    }

    void DeviceVK::wait_idle()
    {
        vkDeviceWaitIdle(m_device);
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

        if (prefer_high_performance) {
            // Find a discrete GPU and chooses it
            for (auto current : physical_devices) {
                VkPhysicalDeviceProperties properties;
                vkGetPhysicalDeviceProperties(current, &properties);

                if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                    physical_device = current;
                    vkGetPhysicalDeviceFeatures(physical_device, &features);
                    break;
                }
            }
        }
        else {
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
                instance, physical_device,
                device, allocator,
                queue_info.queueFamilyIndex));
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
}
