#ifndef _TEMPEH_RENDERER_GUI_RENDERER_HPP
#define _TEMPEH_RENDERER_GUI_RENDERER_HPP

namespace TempehEditor::Renderer::GUI {

	class GUIRenderer
	{
	public:
		virtual ~GUIRenderer() = default;
        virtual void predraw() = 0;
		virtual void draw() = 0;
        virtual void postdraw() = 0;
	};

}

#endif
