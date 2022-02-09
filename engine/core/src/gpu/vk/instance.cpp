#include <tempeh/gpu/vk/device.hpp>
#include <tempeh/gpu/vk/surface.hpp>
#include <tempeh/gpu/vk/vk.hpp>

namespace Tempeh::GPU::Vk
{
    static const char* instance_layers[] = {
        "VK_LAYER_KHRONOS_validation"
    };

    static const char* instance_extensions[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
    };

    RefDeviceResult<Instance> create_instance()
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

        for (const auto layer : instance_layers) {
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
            return parse_error(result);
        }

        volkLoadInstance(instance); // Load instance functions
    }

}
