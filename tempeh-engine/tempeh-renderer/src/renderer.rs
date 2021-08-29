use crate::mini_block::BlockingFuture;
use crate::state::State;
use crate::VERTICES;
use std::iter;
use tempeh_window::ScreenSize;
use wgpu::{
    Color, CommandBuffer, CommandEncoder, CommandEncoderDescriptor, LoadOp, Operations,
    RenderPassColorAttachment, RenderPassDescriptor, SwapChainError,
};

pub struct Renderer {
    pub state: State,
    pub clear_color: Color,
    pub command_buffer_queue: Option<Vec<CommandBuffer>>,
}

impl Renderer {
    pub fn new(
        window: &impl tempeh_window::HasRawWindowHandle,
        screen_size: ScreenSize,
        clear_color: Color,
    ) -> Self {
        Self {
            state: State::new(window, screen_size).block(),
            clear_color,
            command_buffer_queue: Some(vec![]),
        }
    }

    pub fn resize(&mut self, size: ScreenSize) {
        self.state.swapchain_descriptor.width = size.width;
        self.state.swapchain_descriptor.height = size.height;
        self.state.swapchain = self
            .state
            .device
            .create_swap_chain(&self.state.surface, &self.state.swapchain_descriptor)
    }

    // pub fn render(&self) -> Result<(), SwapChainError> {
    //     let swapchain_texture = self.state.swapchain.get_current_frame()?.output;
    //     let mut command_encoder = self
    //         .state
    //         .device
    //         .create_command_encoder(&CommandEncoderDescriptor { label: None });
    //     {
    // let mut render_pass = command_encoder.begin_render_pass(&RenderPassDescriptor {
    //     label: None,
    //     color_attachments: &[RenderPassColorAttachment {
    //         ops: Operations {
    //             store: true,
    //             load: LoadOp::Clear(self.clear_color),
    //         },
    //         resolve_target: None,
    //         view: &swapchain_texture.view,
    //     }],
    //     depth_stencil_attachment: None,
    // });
    // render_pass.set_pipeline(&self.state.render_pipeline);
    // render_pass.set_bind_group(0, &self.state.uniform_bind_group, &[]);
    // render_pass.set_bind_group(1, &self.state.texture_bind_group, &[]);
    // render_pass.set_vertex_buffer(0, self.state.vertex_buffer.slice(..));
    // render_pass.draw(0..VERTICES.len() as u32, 0..1);
    // }
    //     let command_buffer = command_encoder.finish();
    //     self.state.queue.submit(iter::once(command_buffer));
    //
    //     Ok(())
    // }

    pub fn set_clear_color(&mut self, clear_color: Color) {
        self.clear_color = clear_color;
    }
}
