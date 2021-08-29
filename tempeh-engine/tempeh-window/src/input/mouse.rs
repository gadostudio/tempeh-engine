use crate::input::KeyState;

#[derive(Debug, PartialOrd, PartialEq, Eq, Hash, Clone)]
pub struct MouseInput {
    pub state: KeyState,
    pub button: MouseButton,
}

#[derive(Debug, PartialOrd, PartialEq, Eq, Hash, Clone)]
pub enum MouseButton {
    Left,
    Right,
    Middle,
    Other(u16),
}
