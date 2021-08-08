use crate::input::mouse::MouseButton;
use crate::input::touch::TouchPhase;

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
        logical_position: nalgebra::Point2<f64>,
        phase: TouchPhase,
    },
    MouseInput {
        state: KeyState,
        button: MouseButton,
    },
}
