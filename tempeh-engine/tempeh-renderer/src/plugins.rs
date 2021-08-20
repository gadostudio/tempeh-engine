use crate::component_system::sprite_render_system;
use tempeh_core::plugins::Plugin;
use tempeh_core::AppBuilder;

pub struct RendererPlugin {}

impl Default for RendererPlugin {
    fn default() -> Self {
        Self {}
    }
}

impl Plugin for RendererPlugin {
    fn inject(&self, app: &mut AppBuilder) {
        // todo!("System postupdate");
        app.add_system(sprite_render_system());
    }
}
