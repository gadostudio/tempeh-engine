use tempeh_core::plugins::Plugin;
use tempeh_core::AppBuilder;

use crate::component_system::{
    render_system, sprite_render_queue_system, sprite_renderer_initialization_system,
};
use crate::renderer::Renderer;
use crate::sprite::SpriteRenderer;
use tempeh_core_component::Transform;

pub struct RendererPlugin {}

impl Default for RendererPlugin {
    fn default() -> Self {
        Self {}
    }
}

impl<W: tempeh_window::TempehWindow + tempeh_window::Runner> Plugin<W> for RendererPlugin {
    fn inject(&self, app: &mut AppBuilder<W>) {
        app.add_preupdate_system(sprite_renderer_initialization_system());
        app.add_postupdate_system(sprite_render_queue_system());
        // app.add_resource::<Option<Renderer>>(None);
        log::warn!("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa!");
        app.add_postupdate_system(render_system());

        app.add_component((
            SpriteRenderer {
                texture: image::load_from_memory(include_bytes!("../../../assets/image/tree.png"))
                    .unwrap(),
            },
            Transform::default(),
        ));
    }
}
