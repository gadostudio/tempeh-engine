#ifndef _TEMPEH_RENDERER_BACKEND_OPENGL_ES_INSTANCE_HPP
#define _TEMPEH_RENDERER_BACKEND_OPENGL_ES_INSTANCE_HPP

#include <tempeh/renderer/instance.hpp>
#include <glad/glad.h>
#include <sbstd.hpp>

namespace Tempeh::Renderer::Backend::OpenGLES
{

	interface OpenGLInstance : public Instance
	{
	public:
		virtual const Type type() { return Instance::Type::OpenGLES };
		const char* name() { return "OpenGL ES" };
		sbstd::Optional<const char*> version()
		{
			return sbstd::Optional<const char*>{glGetString(GL_VERSION)};
		};
	};

}
