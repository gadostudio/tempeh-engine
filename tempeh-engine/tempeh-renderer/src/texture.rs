use image::{EncodableLayout, GenericImageView};
use wgpu::{
    Extent3d, FilterMode, ImageCopyTexture, ImageDataLayout, SamplerDescriptor, TextureDescriptor,
    TextureDimension, TextureFormat, TextureUsage, TextureViewDescriptor,
};

pub struct Texture {
    pub texture: wgpu::Texture,
    pub texture_view: wgpu::TextureView,
    pub sampler: wgpu::Sampler,
}

impl Texture {
    pub fn from_bytes(device: &wgpu::Device, queue: &wgpu::Queue, buffer: &[u8]) -> Self {
        let img = image::load_from_memory(buffer).unwrap();
        Self::from_image(device, queue, &img)
    }

    pub fn from_image(
        device: &wgpu::Device,
        queue: &wgpu::Queue,
        img: &image::DynamicImage,
    ) -> Self {
        let rgba = img.as_rgba8().unwrap();
        let (width, height) = img.dimensions();
        let size = Extent3d {
            width,
            height,
            depth_or_array_layers: 1,
        };

        let texture = device.create_texture(&TextureDescriptor {
            label: None,
            size,
            mip_level_count: 1,
            sample_count: 1,
            dimension: TextureDimension::D2,
            format: TextureFormat::Rgba8UnormSrgb,
            usage: TextureUsage::SAMPLED | TextureUsage::COPY_DST,
        });

        let texture_view = texture.create_view(&TextureViewDescriptor::default());

        let sampler = device.create_sampler(&SamplerDescriptor {
            label: None,
            address_mode_u: Default::default(),
            address_mode_v: Default::default(),
            address_mode_w: Default::default(),
            mag_filter: FilterMode::Linear,
            min_filter: FilterMode::Nearest,
            mipmap_filter: FilterMode::Nearest,
            ..Default::default()
        });

        queue.write_texture(
            ImageCopyTexture {
                texture: &texture,
                mip_level: 0,
                origin: wgpu::Origin3d::ZERO,
            },
            rgba,
            ImageDataLayout {
                offset: 0,
                bytes_per_row: Some(std::num::NonZeroU32::new(width * 4).unwrap()),
                rows_per_image: Some(std::num::NonZeroU32::new(height).unwrap()),
            },
            size,
        );

        Self {
            texture,
            texture_view,
            sampler,
        }
    }
}
