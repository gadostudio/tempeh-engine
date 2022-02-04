#include "resource_vk.hpp"

namespace Tempeh::GPU
{
    TextureVK::TextureVK(
        DeviceVK* parent_device,
        VkImage image,
        VkImageView view,
        VmaAllocation allocation,
        const TextureDesc& desc)
        : Texture(desc),
          m_parent_device(parent_device),
          m_image(image),
          m_image_view(view),
          m_allocation(allocation)
    {
    }

    TextureVK::~TextureVK()
    {
    }
}
