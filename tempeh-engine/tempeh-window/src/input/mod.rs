use crate::input::mouse::MouseButton;
use crate::input::touch::TouchPhase;
use tempeh_math::prelude::*;

mod keyboard;
pub mod mouse;
pub mod touch;

#[derive(Debug)]
pub enum KeyState {
    Pressed,
    Released,
}

#[derive(Debug)]
pub enum Input {
    TouchInput {
        id: u64,
        logical_position: Point2<f64>,
        phase: TouchPhase,
    },
    MouseInput {
        state: KeyState,
        button: MouseButton,
    },
}
