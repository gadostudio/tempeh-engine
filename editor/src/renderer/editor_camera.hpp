#ifndef _TEMPEH_EDITOR_RENDERER_EDITOR_CAMERA_HPP
#define _TEMPEH_EDITOR_RENDERER_EDITOR_CAMERA_HPP

#include <tempeh/math.hpp>
#include <tempeh/renderer/camera.hpp>

namespace TempehEditor::Renderer
{

	class EditorCamera2D: public Tempeh::Renderer::Camera
	{
	private:
		Tempeh::Math::mat4 view_matrix;
		Tempeh::Math::mat4 projection_matrix;
		Tempeh::Math::mat4 view_projection_matrix;

		f32 zoom = 1.0;
	public:
		EditorCamera2D(f32 aspect_ratio):
			projection_matrix(Tempeh::Math::ortho(-aspect_ratio * zoom, aspect_ratio * zoom, -zoom, zoom, -1.0f, 1.0f))
		{
		}

		const Tempeh::Math::mat4& get_view_projection_matrix() override { return view_projection_matrix; }
		const Tempeh::Math::mat4& get_view_matrix() override { return view_matrix; }
		const Tempeh::Math::mat4& get_projection_matrix() override { return projection_matrix; }
		void recalculate_matrix() override
		{

			view_projection_matrix = projection_matrix * view_matrix;
		}
	};

}

#endif
