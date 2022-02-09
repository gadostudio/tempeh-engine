#ifndef _TEMPEH_RENDERER_ORTHOGONAL_CAMERA_HPP
#define _TEMPEH_RENDERER_ORTHOGONAL_CAMERA_HPP

#include <tempeh/renderer/camera.hpp>
#include <tempeh/math.hpp>

namespace Tempeh::Renderer
{

	class OrthogonalCamera : public Camera
	{
	public:
		void set_camera_position(Tempeh::Math::vec2& position);

		const Math::mat4& get_view_matrix() const;
		const Math::mat4& get_projection_matrix() const;
		const Math::mat4& get_view_projection_matrix() const;

	private:
		Math::vec2 m_position;
		Math::mat4 m_view_matrix;
		Math::mat4 m_projection_matrix;
		Math::mat4 m_combined_matrix;

		void recalculate_matrix();
	};

}

#endif
