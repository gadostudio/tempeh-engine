use crate::{ScreenSize, VERTICES};

pub struct Camera2D {
    dimension_viewport: ScreenSize,
}

impl Camera2D {
    pub fn new(dimension_viewport: ScreenSize) -> Self {
        Self { dimension_viewport }
    }

    pub fn get_matrix(&self) -> nalgebra::Matrix4<f32> {
        nalgebra::Matrix4::from([
            [100.0 / self.dimension_viewport.width as f32, 0.0, 0.0, 0.0],
            [0.0, 100.0 / self.dimension_viewport.height as f32, 0.0, 0.0],
            [0.0, 0.0, 1.0, 0.0],
            [0.0, 0.0, 0.0, 1.0],
        ])
    }
}
