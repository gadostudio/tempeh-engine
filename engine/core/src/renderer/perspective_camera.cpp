#include <tempeh/renderer/camera.hpp>
#include <tempeh/math.hpp>
#include <tempeh/renderer/perspective_camera.hpp>

namespace Tempeh::Renderer
{

	void PerspectiveCamera::recalculate_matrix() {}
	const Tempeh::Math::mat4& PerspectiveCamera::get_view_matrix() { return Tempeh::Math::mat4{}; }
	const Tempeh::Math::mat4& PerspectiveCamera::get_projection_matrix() { return Tempeh::Math::mat4{}; }
	const Tempeh::Math::mat4& PerspectiveCamera::get_view_projection_matrix() { return Tempeh::Math::mat4{}; }
	void PerspectiveCamera::set_camera_position(Tempeh::Math::vec3& pos) {}

}
