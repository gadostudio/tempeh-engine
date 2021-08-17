use crate::input::keyboard::{KeyboardInput, VirtualKeyCode};
use crate::input::mouse::MouseButton;
use crate::input::touch::TouchInput;
use std::collections::hash_set::Iter;
use std::collections::HashSet;
use tempeh_math::prelude::*;

pub mod keyboard;
pub mod mouse;
pub mod touch;

#[derive(Debug, Eq, PartialEq, Hash, Clone)]
pub enum KeyState {
    Pressed,
    Released,
}

#[derive(Debug, Clone)]
pub struct InputManager {
    pub touches: HashSet<TouchInput>,
    pub mouse_button: HashSet<MouseButton>,
    pub keyboard_presses: HashSet<KeyboardInput>,
}

impl InputManager {
    pub fn new() -> Self {
        Self {
            touches: HashSet::new(),
            mouse_button: HashSet::new(),
            keyboard_presses: HashSet::new(),
        }
    }

    pub fn reset(&mut self) {
        self.touches.clear();
        self.mouse_button.clear();
        self.keyboard_presses.clear();
    }

    pub fn touches(&self) -> Iter<'_, TouchInput> {
        self.touches.iter()
    }

    pub fn mouse_buttons(&self) -> Iter<'_, MouseButton> {
        self.mouse_button.iter()
    }

    pub fn keyboard_presses(&self) -> Iter<'_, KeyboardInput> {
        self.keyboard_presses.iter()
    }

    pub fn is_key_pressed(&self, virtual_keycode: VirtualKeyCode) -> bool {
        self.keyboard_presses.iter().any(|key| {
            if let Some(virtual_keycode_) = &key.virtual_keycode {
                virtual_keycode == *virtual_keycode_
            } else {
                false
            }
        })
    }
}
