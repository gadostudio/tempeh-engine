use wgpu::util::{BufferInitDescriptor, DeviceExt};
use wgpu::{
    include_spirv, BindGroup, BindGroupDescriptor, BindGroupEntry, BindGroupLayoutDescriptor,
    BindGroupLayoutEntry, BindingResource, BindingType, BlendState, Buffer, BufferBindingType,
    BufferUsage, ColorTargetState, ColorWrite, CommandBuffer, CommandEncoderDescriptor, Face,
    FragmentState, FrontFace, LoadOp, MultisampleState, Operations, PipelineLayoutDescriptor,
    PolygonMode, PrimitiveState, PrimitiveTopology, RenderPassColorAttachment,
    RenderPassDescriptor, RenderPipeline, RenderPipelineDescriptor, ShaderStage, TextureSampleType,
    TextureViewDimension, VertexState,
};

use tempeh_window::ScreenSize;

use crate::camera::Camera2D;
use crate::command_encoder::CommandBufferGenerator;
use crate::prelude::State;
use crate::renderer::Renderer;
use crate::uniform::Uniform;
use crate::{Vertex, VERTICES};
use std::iter;

pub struct SpriteRenderer {
    pub(crate) texture: image::DynamicImage,
}

pub struct SpriteRendererPipeline {
    render_pipeline: RenderPipeline,
    pub(crate) vertex_buffer: Buffer,
    uniform_bind_group: BindGroup,
    texture_bind_group: BindGroup,
}

impl SpriteRendererPipeline {
    pub fn new(renderer: &Renderer) -> Self {
        let Renderer {
            state:
                State {
                    queue,
                    device,
                    swapchain_descriptor,
                    ..
                },
            ..
        } = renderer;

        let shader_vertex = device.create_shader_module(&include_spirv!("./shaders/test.vert.spv"));
        let shader_fragment =
            device.create_shader_module(&include_spirv!("./shaders/test.frag.spv"));

        let camera = Camera2D::new(ScreenSize {
            width: 720,
            height: 480,
        });
        let camera_uniform = Uniform::new(&camera);
        let uniform_buffer = device.create_buffer_init(&BufferInitDescriptor {
            label: None,
            usage: BufferUsage::UNIFORM | BufferUsage::COPY_SRC,
            contents: bytemuck::cast_slice(&[camera_uniform]),
        });

        let res = crate::texture::Texture::from_image(
            &device,
            &queue,
            &image::load_from_memory(include_bytes!("../../../assets/image/tree.png")).unwrap(),
        );

        let texture_bind_group_layout =
            device.create_bind_group_layout(&BindGroupLayoutDescriptor {
                label: None,
                entries: &[
                    BindGroupLayoutEntry {
                        binding: 0,
                        visibility: ShaderStage::FRAGMENT,
                        ty: BindingType::Texture {
                            sample_type: TextureSampleType::Float { filterable: true },
                            view_dimension: TextureViewDimension::D2,
                            multisampled: false,
                        },
                        count: None,
                    },
                    BindGroupLayoutEntry {
                        binding: 1,
                        visibility: ShaderStage::FRAGMENT,
                        ty: BindingType::Sampler {
                            filtering: true,
                            comparison: false,
                        },
                        count: None,
                    },
                ],
            });
        let texture_bind_group = device.create_bind_group(&BindGroupDescriptor {
            label: None,
            layout: &texture_bind_group_layout,
            entries: &[
                BindGroupEntry {
                    binding: 0,
                    resource: BindingResource::TextureView(&res.texture_view),
                },
                BindGroupEntry {
                    binding: 1,
                    resource: BindingResource::Sampler(&res.sampler),
                },
            ],
        });

        let uniform_bind_group_layout =
            device.create_bind_group_layout(&BindGroupLayoutDescriptor {
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
            layout: &uniform_bind_group_layout,
            entries: &[BindGroupEntry {
                binding: 0,
                resource: uniform_buffer.as_entire_binding(),
            }],
        });

        let render_pipeline_layout = device.create_pipeline_layout(&PipelineLayoutDescriptor {
            label: None,
            bind_group_layouts: &[&uniform_bind_group_layout, &texture_bind_group_layout],
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
                topology: PrimitiveTopology::TriangleStrip,
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
            usage: BufferUsage::VERTEX | BufferUsage::COPY_DST,
            contents: bytemuck::cast_slice(VERTICES),
        });

        Self {
            vertex_buffer,
            texture_bind_group,
            render_pipeline,
            uniform_bind_group,
        }
    }
}

impl CommandBufferGenerator for SpriteRendererPipeline {
    fn command_buffer(&self, renderer: &Renderer) -> bool {
        let swapchain_texture = renderer.state.swapchain.get_current_frame().unwrap().output;
        let mut command_encoder = renderer
            .state
            .device
            .create_command_encoder(&CommandEncoderDescriptor { label: None });
        {
            let mut render_pass = command_encoder.begin_render_pass(&RenderPassDescriptor {
                label: None,
                color_attachments: &[RenderPassColorAttachment {
                    ops: Operations {
                        store: true,
                        load: LoadOp::Clear(renderer.clear_color),
                    },
                    resolve_target: None,
                    view: &swapchain_texture.view,
                }],
                depth_stencil_attachment: None,
            });
            render_pass.set_pipeline(&self.render_pipeline);
            render_pass.set_bind_group(0, &self.uniform_bind_group, &[]);
            render_pass.set_bind_group(1, &self.texture_bind_group, &[]);
            render_pass.set_vertex_buffer(0, self.vertex_buffer.slice(..));
            render_pass.draw(0..VERTICES.len() as u32, 0..1);
        }
        let a = command_encoder.finish();
        renderer.state.queue.submit(iter::once(a));
        true
    }
}
