#include <tempeh/gpu/device.hpp>
#include <tempeh/gpu/surface.hpp>
#include <tempeh/gpu/types.hpp>
#include <tempeh/gpu/vk/device.hpp>
#include <tempeh/gpu/vk/surface.hpp>
#include <tempeh/gpu/vk/vk.hpp>
#include <tempeh/common/os.hpp>
#include <map>
#include <GLFW/glfw3.h>
#include <memory>

#ifdef TEMPEH_OS_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include <GLFW/glfw3native.h>

namespace Tempeh::GPU::Vk
{

    static const char* device_extensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_MAINTENANCE1_EXTENSION_NAME
    };

    Device::Device(
        VkInstance instance,
        VkPhysicalDevice physical_device,
        VkDevice device,
        u32 main_queue_index)
        : m_instance(instance),
          m_physical_device(physical_device),
          m_device(device),
          m_main_queue_index(main_queue_index)
    {
        vkGetDeviceQueue(m_device, m_main_queue_index, 0, &m_main_queue);

        m_cmd_manager = std::make_unique<CommandManagerVK<max_command_buffers>>(
            m_device, m_main_queue_index);
    }

    Device::~Device()
    {
        vkDeviceWaitIdle(m_device);
        m_cmd_manager.reset();
        vkDestroyDevice(m_device, nullptr);
        vkDestroyInstance(m_instance, nullptr);
    }

    RefDeviceResult<GPU::Surface> Device::create_surface(
        const std::shared_ptr<Window::Window>& window,
        const SurfaceDesc& desc)
    {
        VkSurfaceKHR vk_surface = VK_NULL_HANDLE;

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

        Surface* surface = new Surface(vk_surface, this);
        DeviceErrorCode err = surface->initialize(desc);

        if (err != DeviceErrorCode::Ok) {
            delete surface;
            vkDestroySurfaceKHR(m_instance, vk_surface, nullptr);
            return err;
        }

        surface->attach_window(window);

        return Util::Rc<GPU::Surface>(surface);
    }

    RefDeviceResult<GPU::Texture> Device::create_texture(const TextureDesc& desc)
    {
        return DeviceErrorCode::Unimplemented;
    }

    RefDeviceResult<GPU::Buffer> Device::create_buffer(const BufferDesc& desc)
    {
        return DeviceErrorCode::Unimplemented;
    }

    void Device::begin_frame()
    {
    }

    void Device::end_frame()
    {
    }

    void Device::wait_idle()
    {
        vkDeviceWaitIdle(m_device);
    }

    RefDeviceResult<Device> create_device(bool prefer_high_performance)
    {

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

        std::vector<const char*> enabled_exts;
        std::vector<VkExtensionProperties> ext_properties;
        u32 num_extensions;

        // Query physical device extensions
        vkEnumerateDeviceExtensionProperties(
            physical_device, nullptr, &num_extensions, nullptr);

        ext_properties.resize(num_extensions);

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
        device_info.enabledExtensionCount = static_cast<uint32_t>(enabled_exts.size());
        device_info.ppEnabledExtensionNames = enabled_exts.data();
        device_info.pEnabledFeatures = &features;

        VkDevice device = VK_NULL_HANDLE;
        VkResult result = vkCreateDevice(physical_device, &device_info, nullptr, &device);

        if (VULKAN_FAILED(result)) {
            vkDestroyInstance(instance, nullptr);
            return parse_error(result);
        }

        return std::static_pointer_cast<Device>(
            std::make_shared<Device>(
                instance, physical_device,
                device, queue_info.queueFamilyIndex));
    }

    VkSurfaceKHR Device::create_surface_glfw(const std::shared_ptr<Window::Window>& window)
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
