use crate::state::State;
use std::fs::File;
use tempeh_core_component::prelude::*;
use tempeh_ecs::prelude::*;
use wgpu::{BindGroup, Buffer, RenderPipeline};

pub struct SpriteRenderer {
    render_pipeline: RenderPipeline,
    vertex_buffer: Buffer,
    uniform_bind_group: BindGroup,
    texture_bind_group: BindGroup,
}

#[system(for_each)]
pub fn sprite_render(
    sprite_renderer: &SpriteRenderer,
    transform: &Transform,
    // #[resource] renderer: &Renderer,
) {
}
