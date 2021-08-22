use crate::input::keyboard::{KeyboardInput, VirtualKeyCode};
use crate::input::mouse::{MouseButton, MouseInput};
use crate::input::touch::TouchInput;
use std::collections::hash_set::Iter;
use std::collections::{hash_map, HashMap, HashSet};
use tempeh_math::prelude::*;

pub mod keyboard;
pub mod mouse;
pub mod touch;

#[derive(Debug, PartialOrd, PartialEq, Eq, Hash, Clone)]
pub enum KeyState {
    Pressed,
    Released,
}

#[derive(Debug, Clone)]
pub struct InputManager {
    pub touch_presses: HashMap<u64, tempeh_math::Point2<f64>>,
    pub touches: HashMap<u64, TouchInput>,
    pub mouse_button: HashSet<MouseButton>,
    pub mouse_presses: HashSet<MouseInput>,
    pub keyboard_presses: HashSet<KeyboardInput>,
    pub keyboard_key_presses: HashSet<VirtualKeyCode>,
}

impl InputManager {
    pub fn new() -> Self {
        Self {
            touch_presses: HashMap::new(),
            touches: HashMap::new(),
            mouse_button: HashSet::new(),
            mouse_presses: HashSet::new(),
            keyboard_presses: HashSet::new(),
            keyboard_key_presses: HashSet::new(),
        }
    }

    pub fn reset(&mut self) {
        self.touches.clear();
        self.mouse_presses.clear();
        self.keyboard_presses.clear();
    }

    pub fn touches(&self) -> hash_map::Iter<'_, u64, TouchInput> {
        self.touches.iter()
    }

    pub fn mouse_buttons(&self) -> Iter<'_, MouseButton> {
        self.mouse_button.iter()
    }

    pub fn keyboard_pressed_key(&self) -> Iter<'_, VirtualKeyCode> {
        self.keyboard_key_presses.iter()
    }

    pub fn is_key_pressed(&self, virtual_keycode: &VirtualKeyCode) -> bool {
        self.keyboard_key_presses.contains(virtual_keycode)
    }

    pub fn is_mouse_pressed(&self, mouse_button: &MouseButton) -> bool {
        self.mouse_button.contains(mouse_button)
    }
}
