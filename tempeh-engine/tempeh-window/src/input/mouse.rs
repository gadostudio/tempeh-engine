use crate::input::KeyState;

#[derive(Clone)]
struct MouseInput {
    state: KeyState,
    button: MouseButton,
}

#[derive(Debug, Clone)]
pub enum MouseButton {
    Left,
    Right,
    Middle,
    Other(u16),
}
