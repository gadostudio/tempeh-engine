use async_std::task;
use core::iter;
use imgui::{Context, TextureId};
use imgui_wgpu::{Renderer as RendererImGui, RendererConfig};
use imgui_wgpu::{Texture as TextureImGui, TextureConfig};
use wgpu::SurfaceConfiguration;
use winit::dpi::PhysicalSize;

pub(crate) struct WindowSize {
    pub(crate) width: u32,
    pub(crate) height: u32,
}

pub(crate) struct Renderer {
    pub(crate) state: State,
    pub(crate) imgui_renderer: RendererImGui,
    pub(crate) surface: wgpu::Surface,
}

impl Renderer {
    pub(crate) fn new<W: raw_window_handle::HasRawWindowHandle>(
        window: &W,
        imgui_context: &mut Context,
        window_size: &PhysicalSize<u32>,
    ) -> Self {
        let instance = wgpu::Instance::new(wgpu::Backends::PRIMARY);
        let surface = unsafe { instance.create_surface(window) };
        let adapter = task::block_on(instance.request_adapter(&wgpu::RequestAdapterOptions {
            compatible_surface: Some(&surface),
            power_preference: wgpu::PowerPreference::HighPerformance,
        }));
        let (device, queue) = task::block_on(adapter.unwrap().request_device(
            &wgpu::DeviceDescriptor {
                label: None,
                features: wgpu::Features::empty(),
                limits: wgpu::Limits::default(),
            },
            None,
        ))
        .unwrap();
        surface.configure(
            &device,
            &SurfaceConfiguration {
                width: window_size.width,
                height: window_size.height,
                usage: wgpu::TextureUsages::RENDER_ATTACHMENT,
                present_mode: wgpu::PresentMode::Mailbox,
                format: wgpu::TextureFormat::Bgra8Unorm,
            },
        );

        Self {
            imgui_renderer: RendererImGui::new(
                imgui_context,
                &device,
                &queue,
                RendererConfig {
                    texture_format: wgpu::TextureFormat::Bgra8Unorm,
                    ..Default::default()
                },
            ),
            state: State { device, queue },
            surface,
        }
    }
}

pub(crate) struct State {
    pub(crate) device: wgpu::Device,
    pub(crate) queue: wgpu::Queue,
}

pub(crate) struct SceneEditorRendererPipeline {
    pub(crate) texture_id: TextureId,
}

impl SceneEditorRendererPipeline {
    pub(crate) fn new(renderer: &mut Renderer, window_size: &WindowSize) -> Self {
        let texture = Self::create_texture(renderer, window_size);
        let texture_id = renderer.imgui_renderer.textures.insert(texture);

        Self { texture_id }
    }

    pub(crate) fn create_texture(
        renderer: &mut Renderer,
        window_size: &WindowSize,
    ) -> TextureImGui {
        let texture = renderer
            .state
            .device
            .create_texture(&wgpu::TextureDescriptor {
                label: None,
                size: wgpu::Extent3d {
                    depth_or_array_layers: 1,
                    width: window_size.width,
                    height: window_size.height,
                },
                mip_level_count: 1,
                sample_count: 1,
                dimension: wgpu::TextureDimension::D2,
                format: wgpu::TextureFormat::Bgra8Unorm,
                usage: wgpu::TextureUsages::COPY_DST
                    | wgpu::TextureUsages::COPY_SRC
                    | wgpu::TextureUsages::TEXTURE_BINDING
                    | wgpu::TextureUsages::RENDER_ATTACHMENT,
            });

        let texture_view = texture.create_view(&wgpu::TextureViewDescriptor::default());

        let sampler = renderer
            .state
            .device
            .create_sampler(&wgpu::SamplerDescriptor {
                label: None,
                address_mode_u: wgpu::AddressMode::ClampToEdge,
                address_mode_v: wgpu::AddressMode::ClampToEdge,
                address_mode_w: wgpu::AddressMode::ClampToEdge,
                mag_filter: wgpu::FilterMode::Linear,
                min_filter: wgpu::FilterMode::Linear,
                mipmap_filter: wgpu::FilterMode::Linear,
                lod_min_clamp: -100.0,
                lod_max_clamp: 100.0,
                compare: None,
                anisotropy_clamp: None,
                border_color: None,
            });

        let texture_bind_group_layout =
            renderer
                .state
                .device
                .create_bind_group_layout(&wgpu::BindGroupLayoutDescriptor {
                    label: None,
                    entries: &[
                        wgpu::BindGroupLayoutEntry {
                            binding: 0,
                            visibility: wgpu::ShaderStages::FRAGMENT,
                            ty: wgpu::BindingType::Texture {
                                multisampled: false,
                                sample_type: wgpu::TextureSampleType::Float { filterable: true },
                                view_dimension: wgpu::TextureViewDimension::D2,
                            },
                            count: None,
                        },
                        wgpu::BindGroupLayoutEntry {
                            binding: 1,
                            visibility: wgpu::ShaderStages::FRAGMENT,
                            ty: wgpu::BindingType::Sampler {
                                comparison: false,
                                filtering: true,
                            },
                            count: None,
                        },
                    ],
                });

        let texture_bind_group =
            renderer
                .state
                .device
                .create_bind_group(&wgpu::BindGroupDescriptor {
                    label: None,
                    layout: &texture_bind_group_layout,
                    entries: &[
                        wgpu::BindGroupEntry {
                            binding: 0,
                            resource: wgpu::BindingResource::TextureView(&texture_view),
                        },
                        wgpu::BindGroupEntry {
                            binding: 1,
                            resource: wgpu::BindingResource::Sampler(&sampler),
                        },
                    ],
                });

        TextureImGui::from_raw_parts(
            texture,
            texture_view,
            texture_bind_group,
            wgpu::Extent3d {
                height: window_size.height,
                width: window_size.width,
                depth_or_array_layers: 1,
            },
        )
    }
}

impl SceneEditorRendererPipeline {
    pub(crate) fn resize(&self, renderer: &mut Renderer, window_size: &WindowSize) {
        let texture = Self::create_texture(renderer, window_size);
        renderer
            .imgui_renderer
            .textures
            .replace(self.texture_id, texture);
    }

    pub(crate) fn command_buffer(&self, renderer: &Renderer) {
        let texture = renderer
            .imgui_renderer
            .textures
            .get(self.texture_id)
            .unwrap();
        let texture_view = texture.view();

        let mut command_encoder = renderer
            .state
            .device
            .create_command_encoder(&wgpu::CommandEncoderDescriptor { label: None });
        {
            let _render_pass = command_encoder.begin_render_pass(&wgpu::RenderPassDescriptor {
                label: None,
                color_attachments: &[wgpu::RenderPassColorAttachment {
                    view: &texture_view,
                    resolve_target: None,
                    ops: wgpu::Operations {
                        load: wgpu::LoadOp::Clear(wgpu::Color {
                            r: 0.0,
                            g: 0.0,
                            b: 0.0,
                            a: 1.0,
                        }),
                        store: true,
                    },
                }],
                depth_stencil_attachment: None,
            });
            // command_encoder.copy_texture_to_buffer(
            //     wgpu::ImageCopyTexture {
            //         texture: &self.texture,
            //         mip_level: 0,
            //         origin: wgpu::Origin3d::ZERO,
            //     },
            //     wgpu::ImageCopyBuffer {
            //         buffer: & output_buffer,
            //         layout: wgpu::ImageDataLayout {
            //             offset: 0,
            //             bytes_per_row: Some(
            //                 std::num::NonZeroU32::new(
            //                     buffer_dimensions.padded_bytes_per_row as u32,
            //                 )
            //                 .unwrap(),
            //             ),
            //             rows_per_image: None,
            //         },
            //     },
            //     texture_extent,
            // );
        }
        renderer
            .state
            .queue
            .submit(iter::once(command_encoder.finish()));
    }
}
