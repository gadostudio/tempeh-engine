#ifndef _TEMPEH_RENDERER_PERSPECTIVE_CAMERA_HPP
#define _TEMPEH_RENDERER_PERSPECTIVE_CAMERA_HPP

#include <tempeh/renderer/camera.hpp>
#include <tempeh/math.hpp>

namespace Tempeh::Renderer
{

	class PerspectiveCamera : public Camera
	{
	private:
		Tempeh::Math::vec3 m_position;

		void recalculate_matrix();
	public:
		const Tempeh::Math::mat4& get_view_matrix();
		const Tempeh::Math::mat4& get_projection_matrix();
		const Tempeh::Math::mat4& get_view_projection_matrix();
		void set_camera_position(Tempeh::Math::vec3& pos);
	};

}

#endif
