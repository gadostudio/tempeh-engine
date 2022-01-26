#ifndef _TEMPEH_RENDERER_CAMERA_HPP
#define _TEMPEH_RENDERER_CAMERA_HPP

#include <tempeh/common/typedefs.hpp>
#include <tempeh/math.hpp>

namespace Tempeh::Renderer
{

	interface Camera
	{
	private:
		virtual void recalculate_matrix() = 0;
	public:
		virtual const Tempeh::Math::mat4& get_view_matrix() = 0;
		virtual const Tempeh::Math::mat4& get_projection_matrix() = 0;
		virtual const Tempeh::Math::mat4& get_view_projection_matrix() = 0;
	};
}

#endif
