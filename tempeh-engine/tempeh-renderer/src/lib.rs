use wgpu::util::{BufferInitDescriptor, DeviceExt};
use wgpu::{
    Adapter, BackendBit, BindGroup, BindGroupDescriptor, BindGroupEntry, BindGroupLayout,
    BindGroupLayoutDescriptor, BindGroupLayoutEntry, BindingResource, BindingType, BlendState,
    Buffer, BufferAddress, BufferBinding, BufferBindingType, BufferUsage, Color, ColorTargetState,
    ColorWrite, CommandEncoderDescriptor, Device, DeviceDescriptor, Face, Features, FragmentState,
    FrontFace, Instance, Limits, LoadOp, MultisampleState, Operations, PipelineLayoutDescriptor,
    PolygonMode, PowerPreference, PresentMode, PrimitiveState, PrimitiveTopology, Queue,
    RenderPassColorAttachment, RenderPassDescriptor, RenderPipeline, RenderPipelineDescriptor,
    RequestAdapterOptions, RequestAdapterOptionsBase, ShaderFlags, ShaderModuleDescriptor,
    ShaderSource, ShaderStage, Surface, SwapChain, SwapChainDescriptor, SwapChainError,
    TextureFormat, TextureUsage, VertexBufferLayout, VertexState,
};

use tempeh_math::prelude::*;

pub mod camera;
pub mod component_system;
pub mod plugins;
pub mod renderer;
pub mod state;
pub mod texture;
pub mod uniform;

#[repr(C)]
#[derive(Copy, Clone, bytemuck::Pod, bytemuck::Zeroable)]
struct Vertex {
    position: [f32; 3],
    tex_coord: [f32; 2],
}

pub struct ScreenSize {
    pub width: u32,
    pub height: u32,
}

impl Vertex {
    fn desc<'a>() -> VertexBufferLayout<'a> {
        VertexBufferLayout {
            array_stride: std::mem::size_of::<Vertex>() as wgpu::BufferAddress,
            step_mode: wgpu::InputStepMode::Vertex,
            attributes: &[
                wgpu::VertexAttribute {
                    offset: 0,
                    shader_location: 0,
                    format: wgpu::VertexFormat::Float32x3,
                },
                wgpu::VertexAttribute {
                    offset: std::mem::size_of::<[f32; 3]>() as wgpu::BufferAddress,
                    shader_location: 1,
                    format: wgpu::VertexFormat::Float32x2,
                },
            ],
        }
    }
}

const VERTICES: &[Vertex] = &[
    Vertex {
        position: [1.0, 1.0, 0.0],
        tex_coord: [1.0, 1.0],
    },
    Vertex {
        position: [1.0, -1.0, 0.0],
        tex_coord: [1.0, 0.0],
    },
    Vertex {
        position: [-1.0, 1.0, 0.0],
        tex_coord: [0.0, 1.0],
    },
    Vertex {
        position: [-1.0, -1.0, 0.0],
        tex_coord: [0.0, 0.0],
    },
];

pub mod prelude {
    pub use crate::plugins::RendererPlugin;
    pub use crate::state::State;
}
