#include <tempeh/renderer/camera.hpp>
#include <tempeh/math.hpp>
#include <tempeh/renderer/orthogonal_camera.hpp>

namespace Tempeh::Renderer
{

	void OrthogonalCamera::recalculate_matrix() {}
	const Tempeh::Math::mat4& OrthogonalCamera::get_view_matrix() { return Tempeh::Math::mat4{}; }
	const Tempeh::Math::mat4& OrthogonalCamera::get_projection_matrix() { return Tempeh::Math::mat4{}; }
	const Tempeh::Math::mat4& OrthogonalCamera::get_view_projection_matrix() { return Tempeh::Math::mat4{}; }
	void OrthogonalCamera::set_camera_position(Tempeh::Math::vec2& pos) {}

}
