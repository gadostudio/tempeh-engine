use crate::camera::Camera2D;

#[repr(C)]
#[derive(Copy, Clone, bytemuck::Pod, bytemuck::Zeroable)]
pub struct Uniform {
    transform_matrix: [[f32; 4]; 4],
}

impl Uniform {
    pub(crate) fn new(camera: &Camera2D) -> Self {
        Uniform {
            transform_matrix: camera.get_matrix().into(),
        }
    }
}
