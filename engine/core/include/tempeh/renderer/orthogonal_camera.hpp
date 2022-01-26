#ifndef _TEMPEH_RENDERER_ORTHOGONAL_CAMERA_HPP
#define _TEMPEH_RENDERER_ORTHOGONAL_CAMERA_HPP

#include <tempeh/renderer/camera.hpp>
#include <tempeh/math.hpp>

namespace Tempeh::Renderer
{

	class OrthogonalCamera : public Camera
	{
	private:
		Tempeh::Math::vec2 m_position;

		void recalculate_matrix();
	public:
		const Tempeh::Math::mat4& get_view_matrix();
		const Tempeh::Math::mat4& get_projection_matrix();
		const Tempeh::Math::mat4& get_view_projection_matrix();
		void set_camera_position(Tempeh::Math::vec2& position);
	};

}

#endif
