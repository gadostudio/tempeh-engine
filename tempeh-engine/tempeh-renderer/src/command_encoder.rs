use crate::renderer::Renderer;


pub trait CommandBufferGenerator {
    fn command_buffer(&self, renderer: &Renderer);
}
