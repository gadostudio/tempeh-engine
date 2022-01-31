#ifndef _TEMPEH_RENDERER_BACKEND_WEBGPU_INSTANCE_HPP
#define _TEMPEH_RENDERER_BACKEND_WEBGPU_INSTANCE_HPP

#include <tempeh/renderer/instance.hpp>
#include <glad/glad.h>
#include <webgpu/webgpu_cpp.h>
#include <sbstd.hpp>

namespace Tempeh::Renderer::Backend::WebGPU
{

	interface WebGPUInstance : public Instance
	{
	private:
		wgpu::Instance mInstance;
	public:
		WebGPUInstance() : mInstance(std::move(wgpu::CreateInstance())) {}
		virtual const Type type() { return Instance::Type::WebGPU; };
		const char* name() { return "WebGPU"; }
		sbstd::Optional<const char*> version()
		{
			return sbstd::Optional{};					
		};
	};

}
