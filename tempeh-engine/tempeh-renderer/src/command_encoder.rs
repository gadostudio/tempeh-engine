use crate::renderer::Renderer;
use wgpu::{CommandBuffer, CommandEncoder};

pub trait CommandBufferGenerator {
    fn command_buffer(&self, renderer: &Renderer) -> bool;
}
