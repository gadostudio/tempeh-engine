#include <tempeh/renderer/camera.hpp>
#include <tempeh/math.hpp>
#include <tempeh/renderer/orthogonal_camera.hpp>

namespace Tempeh::Renderer
{
	void OrthogonalCamera::set_camera_position(Math::vec2& pos) {}

	const Math::mat4& OrthogonalCamera::get_view_matrix() const
	{
		return m_view_matrix;
	}
	
	const Math::mat4& OrthogonalCamera::get_projection_matrix() const
	{
		return m_projection_matrix;
	}
	
	const Math::mat4& OrthogonalCamera::get_view_projection_matrix() const
	{
		return m_combined_matrix;
	}
	
	void OrthogonalCamera::recalculate_matrix() {}

}
