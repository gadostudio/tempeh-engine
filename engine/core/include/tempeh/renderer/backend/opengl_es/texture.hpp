#include "../texture.hpp"

#include <glad/gles2.h>

namespace Tempeh::Renderer::Backend::OpenGLES
{
	class Texture: public Backend
	{
	private:
		u32 id;
	public:
		enum class Type
		{
			1D = GL_TEXTURE_1D,
			2D = GL_TEXTURE_2D,
			3D = GL_TEXTURE_3D
		};

		Texture(Type type)
		{
			glGenTextures(1, &id);
			glBindTexture(type, id);
		}
	};
}
