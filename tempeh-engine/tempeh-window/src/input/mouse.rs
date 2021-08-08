use crate::input::KeyState;

#[derive(Debug)]
pub enum MouseButton {
    Left,
    Right,
    Middle,
    Other(u16),
}
