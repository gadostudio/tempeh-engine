use tempeh_window::ScreenSize;
use wgpu::{RequestAdapterOptions, SurfaceConfiguration};

pub struct State {
    pub(crate) device: wgpu::Device,
    pub(crate) surface: wgpu::Surface,
    pub(crate) surface_format: wgpu::TextureFormat,
    pub(crate) queue: wgpu::Queue,
    pub(crate) adapter: wgpu::Adapter,
    pub(crate) clear_color: wgpu::Color,
}

impl State {
    pub async fn new(
        window: &impl tempeh_window::HasRawWindowHandle,
        screen_size: ScreenSize,
    ) -> Self {
        let instance = wgpu::Instance::new(if cfg!(target_arch = "wasm32") {
            wgpu::Backends::all()
        } else if cfg!(target_os = "android") {
            wgpu::Backends::GL
        } else {
            wgpu::Backends::PRIMARY
        });
        let surface = unsafe { instance.create_surface(window) };
        let adapter = instance
            .request_adapter(&RequestAdapterOptions {
                compatible_surface: Some(&surface),
                power_preference: wgpu::PowerPreference::LowPower,
                force_fallback_adapter: false,
            })
            .await
            .unwrap();
        let (device, queue) = adapter
            .request_device(
                &wgpu::DeviceDescriptor {
                    label: None,
                    features: if cfg!(android) {
                        wgpu::Features::TEXTURE_COMPRESSION_ETC2
                    } else {
                        wgpu::Features::empty()
                    },
                    limits: wgpu::Limits::default(),
                },
                None,
            )
            .await
            .unwrap();
        // let swapchain_descriptor = SwapChainDescriptor {
        //     format: adapter
        //         .get_swap_chain_preferred_format(&surface)
        //         .unwrap_or(wgpu::TextureFormat::Etc2RgbA1Unorm),
        //     present_mode: wgpu::PresentMode::Fifo,
        //     usage: wgpu::TextureUsages::RENDER_ATTACHMENT,
        //     width: screen_size.width,
        //     height: screen_size.height,
        // };
        // let shader_vertex = device.create_shader_module(&include_spirv!("./shaders/test.vert.spv"));
        // let shader_fragment =
        //     device.create_shader_module(&include_spirv!("./shaders/test.frag.spv"));
        //
        // let camera = Camera2D::new(screen_size);
        // let camera_uniform = Uniform::new(&camera);
        // let uniform_buffer = device.create_buffer_init(&BufferInitDescriptor {
        //     label: None,
        //     usage: BufferUsage::UNIFORM | BufferUsage::COPY_SRC,
        //     contents: bytemuck::cast_slice(&[camera_uniform]),
        // });
        //
        // let res = crate::texture::Texture::from_image(
        //     &device,
        //     &queue,
        //     &image::load_from_memory(include_bytes!("../../../assets/image/tree.png")).unwrap(),
        // );
        // let texture_bind_group_layout =
        //     device.create_bind_group_layout(&BindGroupLayoutDescriptor {
        //         label: None,
        //         entries: &[
        //             BindGroupLayoutEntry {
        //                 binding: 0,
        //                 visibility: ShaderStage::FRAGMENT,
        //                 ty: BindingType::Texture {
        //                     sample_type: TextureSampleType::Float { filterable: true },
        //                     view_dimension: TextureViewDimension::D2,
        //                     multisampled: false,
        //                 },
        //                 count: None,
        //             },
        //             BindGroupLayoutEntry {
        //                 binding: 1,
        //                 visibility: ShaderStage::FRAGMENT,
        //                 ty: BindingType::Sampler {
        //                     filtering: true,
        //                     comparison: false,
        //                 },
        //                 count: None,
        //             },
        //         ],
        //     });
        // let texture_bind_group = device.create_bind_group(&BindGroupDescriptor {
        //     label: None,
        //     layout: &texture_bind_group_layout,
        //     entries: &[
        //         BindGroupEntry {
        //             binding: 0,
        //             resource: BindingResource::TextureView(&res.texture_view),
        //         },
        //         BindGroupEntry {
        //             binding: 1,
        //             resource: BindingResource::Sampler(&res.sampler),
        //         },
        //     ],
        // });
        //
        // let uniform_bind_group_layout =
        //     device.create_bind_group_layout(&BindGroupLayoutDescriptor {
        //         label: None,
        //         entries: &[BindGroupLayoutEntry {
        //             ty: BindingType::Buffer {
        //                 ty: BufferBindingType::Uniform,
        //                 has_dynamic_offset: false,
        //                 min_binding_size: None,
        //             },
        //             count: None,
        //             visibility: ShaderStage::VERTEX,
        //             binding: 0,
        //         }],
        //     });
        // let uniform_bind_group = device.create_bind_group(&BindGroupDescriptor {
        //     label: None,
        //     layout: &uniform_bind_group_layout,
        //     entries: &[BindGroupEntry {
        //         binding: 0,
        //         resource: uniform_buffer.as_entire_binding(),
        //     }],
        // });
        //
        // let render_pipeline_layout = device.create_pipeline_layout(&PipelineLayoutDescriptor {
        //     label: None,
        //     bind_group_layouts: &[&uniform_bind_group_layout, &texture_bind_group_layout],
        //     push_constant_ranges: &[],
        // });
        // let render_pipeline = device.create_render_pipeline(&RenderPipelineDescriptor {
        //     label: None,
        //     layout: Some(&render_pipeline_layout),
        //     depth_stencil: None,
        //     multisample: MultisampleState {
        //         alpha_to_coverage_enabled: false,
        //         count: 1,
        //         mask: !0,
        //     },
        //     fragment: Some(FragmentState {
        //         entry_point: "main",
        //         module: &shader_fragment,
        //         targets: &[ColorTargetState {
        //             format: swapchain_descriptor.format,
        //             blend: Some(BlendState::REPLACE),
        //             write_mask: ColorWrite::ALL,
        //         }],
        //     }),
        //     vertex: VertexState {
        //         module: &shader_vertex,
        //         entry_point: "main",
        //         buffers: &[Vertex::desc()],
        //     },
        //     primitive: PrimitiveState {
        //         topology: PrimitiveTopology::TriangleStrip,
        //         clamp_depth: false,
        //         polygon_mode: PolygonMode::Fill,
        //         conservative: false,
        //         cull_mode: Some(Face::Back),
        //         front_face: FrontFace::Ccw,
        //         strip_index_format: None,
        //     },
        // });
        //
        // let vertex_buffer = device.create_buffer_init(&BufferInitDescriptor {
        //     label: None,
        //     usage: BufferUsage::VERTEX,
        //     contents: bytemuck::cast_slice(VERTICES),
        // });

        // let swapchain = device.create_swap_chain(&surface, &swapchain_descriptor);

        let surface_format = surface
            .get_preferred_format(&adapter)
            .unwrap_or(wgpu::TextureFormat::Bgra8Unorm);

        surface.configure(
            &device,
            &SurfaceConfiguration {
                format: surface_format,
                present_mode: wgpu::PresentMode::Mailbox,
                usage: wgpu::TextureUsages::RENDER_ATTACHMENT,
                width: screen_size.width,
                height: screen_size.height,
            },
        );

        Self {
            // swapchain_texture_view: swapchain.get_current_frame().unwrap().output.view,
            // swapchain,
            surface_format,
            adapter,
            surface,
            // swapchain_descriptor,
            device,
            queue,
            // render_pipeline,
            // vertex_buffer,
            // uniform_bind_group,
            // texture_bind_group,
            clear_color: wgpu::Color {
                r: 0.8,
                g: 0.8,
                b: 0.8,
                a: 1.0,
            },
        }
    }

    // pub fn render(&self) -> Result<(), SwapChainError> {
    //     let swapchain_texture = self.swapchain.get_current_frame()?.output;
    //     let mut command_encoder = self
    //         .device
    //         .create_command_encoder(&CommandEncoderDescriptor { label: None });
    //     {
    //         let mut render_pass = command_encoder.begin_render_pass(&RenderPassDescriptor {
    //             label: None,
    //             color_attachments: &[RenderPassColorAttachment {
    //                 ops: Operations {
    //                     store: true,
    //                     load: LoadOp::Clear(self.clear_color),
    //                 },
    //                 resolve_target: None,
    //                 view: &swapchain_texture.view,
    //             }],
    //             depth_stencil_attachment: None,
    //         });
    //         render_pass.set_pipeline(&self.render_pipeline);
    //         render_pass.set_bind_group(0, &self.uniform_bind_group, &[]);
    //         render_pass.set_bind_group(1, &self.texture_bind_group, &[]);
    //         render_pass.set_vertex_buffer(0, self.vertex_buffer.slice(..));
    //         render_pass.draw(0..VERTICES.len() as u32, 0..1);
    //     }
    //     let command_buffer = command_encoder.finish();
    //     self.queue.submit(iter::once(command_buffer));
    //
    //     Ok(())
    // }

    pub fn resize(&mut self, _size: ScreenSize) {
        // self.swapchain_descriptor.width = size.width;
        // self.swapchain_descriptor.height = size.height;
        // self.swapchain = self
        //     .device
        //     .create_swap_chain(&self.surface, &self.swapchain_descriptor)
    }

    pub fn set_clear_color(&mut self, clear_color: wgpu::Color) {
        self.clear_color = clear_color;
    }
}
