#ifndef _TEMPEH_RENDERER_BACKEND_BACKEND_HPP
#define _TEMPEH_RENDERER_BACKEND_BACKEND_HPP

#include <typedefs.hpp>
#include <sbstd.hpp>
#include "open_gl/instance.hpp"
#include "vulkan/instance.hpp"

namespace Tempeh::Renderer::Backend {

	enum class Type : u8
	{
		None,
		WebGPU,
		OpenGLES,
		Vulkan,
	};

	INTERFACE class Instance
	{
	public:
		virtual const Type type() = 0;
		virtual const char* name() = 0;
		virtual sbstd::Optional<const char*> version() = 0;
	};

}

#endif
