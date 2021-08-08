mod camera;

use crate::camera::Camera2D;
use nalgebra::{Point3, Vector3};
use std::iter;
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

#[repr(C)]
#[derive(Copy, Clone, bytemuck::Pod, bytemuck::Zeroable)]
struct Vertex {
    position: [f32; 3],
    color: [f32; 3],
}

#[repr(C)]
#[derive(Copy, Clone, bytemuck::Pod, bytemuck::Zeroable)]
struct Uniform {
    transform_matrix: [[f32; 4]; 4],
}

impl Uniform {
    fn new(camera: &Camera2D) -> Self {
        Uniform {
            transform_matrix: camera.get_matrix().into(),
        }
    }
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
                    offset: std::mem::size_of::<[f32; 3]>() as BufferAddress,
                    shader_location: 1,
                    format: wgpu::VertexFormat::Float32x3,
                },
            ],
        }
    }
}

const VERTICES: &[Vertex] = &[
    Vertex {
        position: [0.0, 1.0, 0.0],
        color: [1.0, 0.0, 0.0],
    },
    Vertex {
        position: [-1.0, -1.0, 0.0],
        color: [0.0, 1.0, 0.0],
    },
    Vertex {
        position: [1.0, -1.0, 0.0],
        color: [0.0, 0.0, 1.0],
    },
];

pub struct Renderer {
    device: Device,
    surface: Surface,
    swapchain: SwapChain,
    swapchain_descriptor: SwapChainDescriptor,
    queue: Queue,
    render_pipeline: RenderPipeline,
    vertex_buffer: Buffer,
    uniform_bind_group: BindGroup,
    clear_color: Color,
}

impl Renderer {
    pub async fn new(
        window: &impl raw_window_handle::HasRawWindowHandle,
        screen_size: ScreenSize,
    ) -> Self {
        let instance = Instance::new(BackendBit::PRIMARY);
        let surface = unsafe { instance.create_surface(window) };
        let adapter = instance
            .request_adapter(&RequestAdapterOptions {
                compatible_surface: Some(&surface),
                power_preference: PowerPreference::LowPower,
            })
            .await
            .unwrap();
        let (device, queue) = adapter
            .request_device(
                &DeviceDescriptor {
                    label: Some("WGPU Playground"),
                    features: if cfg!(android) {
                        Features::TEXTURE_COMPRESSION_ETC2
                    } else {
                        Features::empty()
                    },
                    limits: Limits::default(),
                },
                None,
            )
            .await
            .unwrap();
        let swapchain_descriptor = SwapChainDescriptor {
            format: adapter
                .get_swap_chain_preferred_format(&surface)
                .unwrap_or(TextureFormat::EtcRgUnorm),
            present_mode: PresentMode::Fifo,
            usage: TextureUsage::RENDER_ATTACHMENT,
            width: screen_size.width,
            height: screen_size.height,
        };
        let shader_vertex =
            device.create_shader_module(&wgpu::include_spirv!("./shaders/test.vert.spv"));
        let shader_fragment =
            device.create_shader_module(&wgpu::include_spirv!("./shaders/test.frag.spv"));

        let camera = Camera2D::new(screen_size);
        let camera_uniform = Uniform::new(&camera);
        let uniform_buffer = device.create_buffer_init(&BufferInitDescriptor {
            label: None,
            usage: BufferUsage::UNIFORM | BufferUsage::COPY_SRC,
            contents: bytemuck::cast_slice(&[camera_uniform]),
        });
        let bind_group_layout = device.create_bind_group_layout(&BindGroupLayoutDescriptor {
            label: None,
            entries: &[BindGroupLayoutEntry {
                ty: BindingType::Buffer {
                    ty: BufferBindingType::Uniform,
                    has_dynamic_offset: false,
                    min_binding_size: None,
                },
                count: None,
                visibility: ShaderStage::VERTEX,
                binding: 0,
            }],
        });

        let uniform_bind_group = device.create_bind_group(&BindGroupDescriptor {
            label: None,
            layout: &bind_group_layout,
            entries: &[BindGroupEntry {
                binding: 0,
                resource: uniform_buffer.as_entire_binding(),
            }],
        });

        let render_pipeline_layout = device.create_pipeline_layout(&PipelineLayoutDescriptor {
            label: None,
            bind_group_layouts: &[&bind_group_layout],
            push_constant_ranges: &[],
        });
        let render_pipeline = device.create_render_pipeline(&RenderPipelineDescriptor {
            label: None,
            layout: Some(&render_pipeline_layout),
            depth_stencil: None,
            multisample: MultisampleState {
                alpha_to_coverage_enabled: false,
                count: 1,
                mask: !0,
            },
            fragment: Some(FragmentState {
                entry_point: "main",
                module: &shader_fragment,
                targets: &[ColorTargetState {
                    format: swapchain_descriptor.format,
                    blend: Some(BlendState::REPLACE),
                    write_mask: ColorWrite::ALL,
                }],
            }),
            vertex: VertexState {
                module: &shader_vertex,
                entry_point: "main",
                buffers: &[Vertex::desc()],
            },
            primitive: PrimitiveState {
                topology: PrimitiveTopology::TriangleList,
                clamp_depth: false,
                polygon_mode: PolygonMode::Fill,
                conservative: false,
                cull_mode: Some(Face::Back),
                front_face: FrontFace::Ccw,
                strip_index_format: None,
            },
        });

        let vertex_buffer = device.create_buffer_init(&BufferInitDescriptor {
            label: None,
            usage: BufferUsage::VERTEX,
            contents: bytemuck::cast_slice(VERTICES),
        });

        Self {
            swapchain: device.create_swap_chain(&surface, &swapchain_descriptor),
            surface,
            swapchain_descriptor,
            device,
            queue,
            render_pipeline,
            vertex_buffer,
            uniform_bind_group,
            clear_color: Color {
                r: 0.0,
                g: 0.0,
                b: 0.0,
                a: 1.0,
            },
        }
    }

    pub fn resize(&mut self, size: ScreenSize) {
        self.swapchain_descriptor.width = size.width;
        self.swapchain_descriptor.height = size.height;
        self.swapchain = self
            .device
            .create_swap_chain(&self.surface, &self.swapchain_descriptor)
    }

    pub fn render(&self) -> Result<(), SwapChainError> {
        let swapchain_texture = self.swapchain.get_current_frame()?.output;
        let mut command_encoder = self
            .device
            .create_command_encoder(&CommandEncoderDescriptor { label: None });
        {
            let mut render_pass = command_encoder.begin_render_pass(&RenderPassDescriptor {
                label: None,
                color_attachments: &[RenderPassColorAttachment {
                    ops: Operations {
                        store: true,
                        load: LoadOp::Clear(self.clear_color),
                    },
                    resolve_target: None,
                    view: &swapchain_texture.view,
                }],
                depth_stencil_attachment: None,
            });
            render_pass.set_pipeline(&self.render_pipeline);
            render_pass.set_bind_group(0, &self.uniform_bind_group, &[]);
            render_pass.set_vertex_buffer(0, self.vertex_buffer.slice(..));
            render_pass.draw(0..VERTICES.len() as u32, 0..1);
        }
        let command_buffer = command_encoder.finish();
        self.queue.submit(iter::once(command_buffer));

        Ok(())
    }

    pub fn set_clear_color(&mut self, clear_color: Color) {
        self.clear_color = clear_color;
    }
}
