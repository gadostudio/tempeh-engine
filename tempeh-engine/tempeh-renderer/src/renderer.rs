// use crate::state::State;
// use crate::ScreenSize;
// use wgpu::{Color, SwapChainError};
//
// struct Renderer {
//     pub state: State,
//     pub clear_color: Color,
// }
//
// impl Renderer {
//     pub fn new(
//         window: &impl raw_window_handle::HasRawWindowHandle,
//         screen_size: ScreenSize,
//     ) -> Self {
//         Self {
//             state: State::new(window, screen_size),
//         }
//     }
//
//     pub fn resize(&mut self, size: ScreenSize) {
//         self.state.swapchain_descriptor.width = size.width;
//         self.state.swapchain_descriptor.height = size.height;
//         self.state.swapchain = self
//             .device
//             .create_swap_chain(&self.surface, &self.swapchain_descriptor)
//     }
//
//     pub fn render(&self) -> Result<(), SwapChainError> {
//         let swapchain_texture = self.swapchain.get_current_frame()?.output;
//         let mut command_encoder = self
//             .device
//             .create_command_encoder(&CommandEncoderDescriptor { label: None });
//         {
//             let mut render_pass = command_encoder.begin_render_pass(&RenderPassDescriptor {
//                 label: None,
//                 color_attachments: &[RenderPassColorAttachment {
//                     ops: Operations {
//                         store: true,
//                         load: LoadOp::Clear(self.clear_color),
//                     },
//                     resolve_target: None,
//                     view: &swapchain_texture.view,
//                 }],
//                 depth_stencil_attachment: None,
//             });
//             render_pass.set_pipeline(&self.render_pipeline);
//             render_pass.set_bind_group(0, &self.uniform_bind_group, &[]);
//             render_pass.set_bind_group(1, &self.texture_bind_group, &[]);
//             render_pass.set_vertex_buffer(0, self.vertex_buffer.slice(..));
//             render_pass.draw(0..VERTICES.len() as u32, 0..1);
//         }
//         let command_buffer = command_encoder.finish();
//         self.queue.submit(iter::once(command_buffer));
//
//         Ok(())
//     }
//
//     pub fn set_clear_color(&mut self, clear_color: Color) {
//         self.clear_color = clear_color;
//     }
// }
