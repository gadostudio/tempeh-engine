use crate::VERTICES;
use tempeh_math::prelude::*;
use tempeh_window::ScreenSize;

pub struct Camera2D {
    dimension_viewport: ScreenSize,
}

impl Camera2D {
    pub fn new(dimension_viewport: ScreenSize) -> Self {
        Self { dimension_viewport }
    }

    pub fn get_matrix(&self) -> Matrix4<f32> {
        Matrix4::from([
            [100.0 / self.dimension_viewport.width as f32, 0.0, 0.0, 0.0],
            [
                0.0,
                -100.0 / self.dimension_viewport.height as f32,
                0.0,
                0.0,
            ],
            [0.0, 0.0, 1.0, 0.0],
            [0.0, 0.0, 0.0, 1.0],
        ])
    }
}
