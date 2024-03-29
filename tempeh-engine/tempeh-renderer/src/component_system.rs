use tempeh_core_component::prelude::*;
use tempeh_ecs::prelude::*;
use tempeh_ecs::Entity;



use crate::command_encoder::CommandBufferGenerator;
use crate::renderer::Renderer;
use crate::sprite::{SpriteRenderer, SpriteRendererPipeline};



use crate::{Vertex};

use tempeh_ecs::systems::CommandBuffer;



#[system]
pub fn render(#[resource] _renderer: &mut Renderer) {
    // renderer
    //     .state
    //     .queue
    //     .submit(renderer.command_buffer_queue.take().unwrap().into_iter());
    // renderer.command_buffer_queue = Some(vec![]);
}

#[system(for_each)]
pub fn sprite_renderer_initialization(
    entity: &Entity,
    command: &mut CommandBuffer,
    _sprite_renderer: &mut SpriteRenderer,
    #[resource] renderer: &Renderer,
) {
    command.add_component(*entity, SpriteRendererPipeline::new(renderer));
    command.remove_component::<SpriteRenderer>(*entity);
}

#[system(for_each)]
pub fn sprite_render_queue(
    sprite_renderer: &SpriteRendererPipeline,
    transform: &mut Transform,
    #[resource] renderer: &mut Renderer,
) {
    log::warn!("render_queue");
    renderer.state.queue.write_buffer(
        &sprite_renderer.vertex_buffer,
        0,
        bytemuck::cast_slice(&[
            Vertex {
                position: [1.0 + transform.position.x, 1.0 - transform.position.y, 0.0],
                tex_coord: [1.0, 1.0],
            },
            Vertex {
                position: [1.0 + transform.position.x, -1.0 - transform.position.y, 0.0],
                tex_coord: [1.0, 0.0],
            },
            Vertex {
                position: [-1.0 + transform.position.x, 1.0 - transform.position.y, 0.0],
                tex_coord: [0.0, 1.0],
            },
            Vertex {
                position: [
                    -1.0 + transform.position.x,
                    -1.0 - transform.position.y,
                    0.0,
                ],
                tex_coord: [0.0, 0.0],
            },
        ]),
    );
    sprite_renderer.command_buffer(renderer);
    // renderer
    //     .command_buffer_queue
    //     .as_mut()
    //     .unwrap()
    //     .push(command_buffer);
}
