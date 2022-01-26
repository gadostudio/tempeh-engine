#ifndef _TEMPEH_RENDERER_BACKEND_VULKAN_INSTANCE_HPP
#define _TEMPEH_RENDERER_BACKEND_VULKAN_INSTANCE_HPP

#include <tempeh/renderer/instance.hpp>
#include <glad/glad.h>
#include <vulkan/vulkan.hpp>
#include <sbstd.hpp>

namespace Tempeh::Renderer::Backend::Vulkan
{

	interface VulkanInstance : public Instance
	{
	private:
		vk::Instance mInstance;
	public:
		VulkanInstance() : mInstance{  } {}
		virtual const Type type() { return Instance::Type::Vulkan; };
		const char* name() { return "Vulkan" };
		sbstd::Optional<const char*> version() { return glGetString(GL_VERSION); };
	};

}
