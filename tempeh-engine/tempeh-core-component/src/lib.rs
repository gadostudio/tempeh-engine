use tempeh_math::prelude::*;

pub struct Transform {
    pub position: Point2<f32>,
    pub scale: Point2<f32>,
    pub rotation: f32,
}

impl Default for Transform {
    fn default() -> Self {
        Self {
            position: Point2::new(0.0, 0.0),
            scale: Point2::new(0.0, 0.0),
            rotation: 0.0,
        }
    }
}

pub mod prelude {
    pub use crate::Transform;
}
