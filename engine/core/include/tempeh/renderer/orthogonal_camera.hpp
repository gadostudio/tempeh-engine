#ifndef _TEMPEH_RENDERER_ORTHOGONAL_CAMERA_HPP
#define _TEMPEH_RENDERER_ORTHOGONAL_CAMERA_HPP

#include <tempeh/renderer/camera.hpp>
#include <tempeh/math.hpp>

namespace Tempeh::Renderer
{

	class OrthogonalCamera: public Camera
	{
	private:
		void recalculate_matrix();
	public:
		virtual const Tempeh::Math::mat4& get_view_matrix();
		virtual const Tempeh::Math::mat4& get_projection_matrix();
		virtual const Tempeh::Math::mat4& get_view_projection_matrix();
	};
}

#endif
