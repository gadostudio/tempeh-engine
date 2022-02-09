#include <tempeh/renderer/camera.hpp>
#include <tempeh/math.hpp>
#include <tempeh/renderer/perspective_camera.hpp>

namespace Tempeh::Renderer
{
	void PerspectiveCamera::set_camera_position(Tempeh::Math::vec3& pos) {}

	const Math::mat4& PerspectiveCamera::get_view_matrix() const
	{
		return m_view_matrix;
	}

	const Math::mat4& PerspectiveCamera::get_projection_matrix() const
	{
		return m_projection_matrix;
	}

	const Math::mat4& PerspectiveCamera::get_view_projection_matrix() const
	{
		return m_combined_matrix;
	}

	void PerspectiveCamera::recalculate_matrix() {}

}
