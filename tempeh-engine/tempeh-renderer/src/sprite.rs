use wgpu::util::{BufferInitDescriptor, DeviceExt};

use tempeh_window::ScreenSize;

use crate::camera::Camera2D;
use crate::command_encoder::CommandBufferGenerator;
use crate::prelude::State;
use crate::renderer::Renderer;
use crate::uniform::Uniform;
use crate::{Vertex, VERTICES};

pub struct SpriteRenderer {
    pub(crate) texture: image::DynamicImage,
}

pub struct SpriteRendererPipeline {
    render_pipeline: wgpu::RenderPipeline,
    pub(crate) vertex_buffer: wgpu::Buffer,
    uniform_bind_group: wgpu::BindGroup,
    texture_bind_group: wgpu::BindGroup,
}

impl SpriteRendererPipeline {
    pub fn new(renderer: &Renderer) -> Self {
        let Renderer {
            state: State { queue, device, .. },
            ..
        } = renderer;

        let shader_vertex =
            device.create_shader_module(&wgpu::include_spirv!("./shaders/test.vert.spv"));
        let shader_fragment =
            device.create_shader_module(&wgpu::include_spirv!("./shaders/test.frag.spv"));

        let camera = Camera2D::new(ScreenSize {
            width: 720,
            height: 480,
        });
        let camera_uniform = Uniform::new(&camera);
        let uniform_buffer = device.create_buffer_init(&BufferInitDescriptor {
            label: None,
            usage: wgpu::BufferUsages::UNIFORM | wgpu::BufferUsages::COPY_SRC,
            contents: bytemuck::cast_slice(&[camera_uniform]),
        });

        let res = crate::texture::Texture::from_image(
            &device,
            &queue,
            &image::load_from_memory(include_bytes!("../../../assets/image/tree.png")).unwrap(),
        );

        let texture_bind_group_layout =
            device.create_bind_group_layout(&wgpu::BindGroupLayoutDescriptor {
                label: None,
                entries: &[
                    wgpu::BindGroupLayoutEntry {
                        binding: 0,
                        visibility: wgpu::ShaderStages::FRAGMENT,
                        ty: wgpu::BindingType::Texture {
                            sample_type: wgpu::TextureSampleType::Float { filterable: true },
                            view_dimension: wgpu::TextureViewDimension::D2,
                            multisampled: false,
                        },
                        count: None,
                    },
                    wgpu::BindGroupLayoutEntry {
                        binding: 1,
                        visibility: wgpu::ShaderStages::FRAGMENT,
                        ty: wgpu::BindingType::Sampler {
                            filtering: true,
                            comparison: false,
                        },
                        count: None,
                    },
                ],
            });
        let texture_bind_group = device.create_bind_group(&wgpu::BindGroupDescriptor {
            label: None,
            layout: &texture_bind_group_layout,
            entries: &[
                wgpu::BindGroupEntry {
                    binding: 0,
                    resource: wgpu::BindingResource::TextureView(&res.texture_view),
                },
                wgpu::BindGroupEntry {
                    binding: 1,
                    resource: wgpu::BindingResource::Sampler(&res.sampler),
                },
            ],
        });

        let uniform_bind_group_layout =
            device.create_bind_group_layout(&wgpu::BindGroupLayoutDescriptor {
                label: None,
                entries: &[wgpu::BindGroupLayoutEntry {
                    ty: wgpu::BindingType::Buffer {
                        ty: wgpu::BufferBindingType::Uniform,
                        has_dynamic_offset: false,
                        min_binding_size: None,
                    },
                    count: None,
                    visibility: wgpu::ShaderStages::VERTEX,
                    binding: 0,
                }],
            });
        let uniform_bind_group = device.create_bind_group(&wgpu::BindGroupDescriptor {
            label: None,
            layout: &uniform_bind_group_layout,
            entries: &[wgpu::BindGroupEntry {
                binding: 0,
                resource: uniform_buffer.as_entire_binding(),
            }],
        });

        let render_pipeline_layout =
            device.create_pipeline_layout(&wgpu::PipelineLayoutDescriptor {
                label: None,
                bind_group_layouts: &[&uniform_bind_group_layout, &texture_bind_group_layout],
                push_constant_ranges: &[],
            });
        let render_pipeline = device.create_render_pipeline(&wgpu::RenderPipelineDescriptor {
            label: None,
            layout: Some(&render_pipeline_layout),
            depth_stencil: None,
            multisample: wgpu::MultisampleState {
                alpha_to_coverage_enabled: false,
                count: 1,
                mask: !0,
            },
            fragment: Some(wgpu::FragmentState {
                entry_point: "main",
                module: &shader_fragment,
                targets: &[wgpu::ColorTargetState {
                    format: renderer.state.surface_format,
                    blend: Some(wgpu::BlendState::REPLACE),
                    write_mask: wgpu::ColorWrites::ALL,
                }],
            }),
            vertex: wgpu::VertexState {
                module: &shader_vertex,
                entry_point: "main",
                buffers: &[Vertex::desc()],
            },
            primitive: wgpu::PrimitiveState {
                topology: wgpu::PrimitiveTopology::TriangleStrip,
                clamp_depth: false,
                polygon_mode: wgpu::PolygonMode::Fill,
                conservative: false,
                cull_mode: Some(wgpu::Face::Back),
                front_face: wgpu::FrontFace::Ccw,
                strip_index_format: None,
            },
        });

        let vertex_buffer = device.create_buffer_init(&BufferInitDescriptor {
            label: None,
            usage: wgpu::BufferUsages::VERTEX | wgpu::BufferUsages::COPY_DST,
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
    fn command_buffer(&self, renderer: &Renderer) {
        let frame = &renderer.state.surface.get_current_frame().unwrap().output;
        let view = &frame
            .texture
            .create_view(&wgpu::TextureViewDescriptor::default());
        let mut command_encoder = renderer
            .state
            .device
            .create_command_encoder(&wgpu::CommandEncoderDescriptor { label: None });
        {
            let mut render_pass = command_encoder.begin_render_pass(&wgpu::RenderPassDescriptor {
                label: None,
                color_attachments: &[wgpu::RenderPassColorAttachment {
                    ops: wgpu::Operations {
                        store: true,
                        load: wgpu::LoadOp::Clear(renderer.clear_color),
                    },
                    resolve_target: None,
                    view,
                }],
                depth_stencil_attachment: None,
            });
            render_pass.set_pipeline(&self.render_pipeline);
            render_pass.set_bind_group(0, &self.uniform_bind_group, &[]);
            render_pass.set_bind_group(1, &self.texture_bind_group, &[]);
            render_pass.set_vertex_buffer(0, self.vertex_buffer.slice(..));
            render_pass.draw(0..VERTICES.len() as u32, 0..1);
        }
        renderer.state.queue.submit(Some(command_encoder.finish()));
    }
}
