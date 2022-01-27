#ifndef _TEMPEH_RENDERER_BACKEND_OPENGL_ES_INSTANCE_HPP
#define _TEMPEH_RENDERER_BACKEND_OPENGL_ES_INSTANCE_HPP

#include <tempeh/renderer/instance.hpp>
#include <glad/glad.h>

namespace Tempeh::Renderer::Backend::OpenGLES
{

	interface OpenGLInstance: public Instance
	{
	public:
		virtual const Type type() { return Instance::Type::OpenGLES };
		const char* name() { return "OpenGL ES" };
		const char* version() { return glGetString(GL_VERSION); };
	};

}

#endif