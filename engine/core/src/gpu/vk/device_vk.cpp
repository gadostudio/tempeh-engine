#include "device_vk.hpp"
#include "vk.hpp"

#include <map>

namespace Tempeh::GPU
{
    static const char* layers[] = {
        "VK_LAYER_KHRONOS_validation"
    };

    static const char* instance_extensions[] = {
        "VK_KHR_surface",
        "VK_KHR_win32_surface"
    };

    static const char* device_extensions[] = {
        "VK_KHR_maintenance1"
    };

    DeviceVK::DeviceVK(
        VkInstance instance,
        VkPhysicalDevice physical_device,
        VkDevice device)
        : m_instance(instance),
          m_physical_device(physical_device),
          m_device(device)
    {
    }

    DeviceVK::~DeviceVK()
    {
        vkDeviceWaitIdle(m_device);
        vkDestroyDevice(m_device, nullptr);
        vkDestroyInstance(m_instance, nullptr);
    }

    RefDeviceResult<Surface> DeviceVK::create_surface(const SurfaceDesc& desc)
    {
        return DeviceErrorCode::Unimplemented;
    }

    RefDeviceResult<Texture> DeviceVK::create_texture(const TextureDesc& desc)
    {
        return DeviceErrorCode::Unimplemented;
    }

    RefDeviceResult<Buffer> DeviceVK::create_buffer(const BufferDesc& desc)
    {
        return DeviceErrorCode::Unimplemented;
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
        instance_info.enabledLayerCount = enabled_layers.size();
        instance_info.ppEnabledLayerNames = enabled_layers.data();
        instance_info.enabledExtensionCount = enabled_exts.size();
        instance_info.ppEnabledExtensionNames = enabled_exts.data();

        VkInstance instance;
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

        // For now, we expect the hardware supports Graphics, Compute and Present queues
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
        device_info.enabledExtensionCount = enabled_exts.size();
        device_info.ppEnabledExtensionNames = enabled_exts.data();
        device_info.pEnabledFeatures = &features;

        VkDevice device;
        result = vkCreateDevice(physical_device, &device_info, nullptr, &device);

        if (VULKAN_FAILED(result)) {
            vkDestroyInstance(instance, nullptr);
            return parse_error_vk(result);
        }

        return std::static_pointer_cast<Device>(
            std::make_shared<DeviceVK>(instance, physical_device, device));
    }
}
