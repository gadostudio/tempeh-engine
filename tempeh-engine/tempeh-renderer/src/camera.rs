use tempeh_math::prelude::*;
use tempeh_math::{Orthographic3, Point3};
use tempeh_window::ScreenSize;

pub struct Camera2D {
    dimension_viewport: ScreenSize,
    zoom: f32,
}

impl Camera2D {
    pub fn new(dimension_viewport: ScreenSize) -> Self {
        Self {
            dimension_viewport,
            zoom: 1f32,
        }
    }

    pub fn set_zoom(&mut self, _zoom: f32) {
        unimplemented!()
    }

    pub fn get_matrix(&self) -> Matrix4<f32> {
        let b = Matrix4::from([
            [100.0 / self.dimension_viewport.width as f32, 0.0, 0.0, 0.0],
            [
                0.0,
                -100.0 / self.dimension_viewport.height as f32,
                0.0,
                0.0,
            ],
            [0.0, 0.0, 1.0, 0.0],
            [0.0, 0.0, 0.0, 1.0],
        ]);
        // println!("{:?}", b);
        // let a = Orthographic3::<f32>::new(-100.0, 100.0, -100.0, 100.0, 10.0, 900.0)
        //     .as_matrix()
        //     .into_owned();
        // let c = Matrix4::new(
        //     1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.5, 1.0,
        // ) * a
        //     * Matrix4::look_at_rh(
        //         &Point3::new(0.0, 0.0, -10.0),
        //         &Point3::new(0.0, 0.0, 0.0),
        //         &Vector3::<f32>::new(0.0, 1.0, 0.0),
        //     );
        // println!("{:?}", c);
        b
    }
}
