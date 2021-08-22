use tempeh_window::input::keyboard::KeyboardInput;
use tempeh_window::input::{
    keyboard::VirtualKeyCode as VirtualKeyCodeTempeh, mouse::MouseButton as MouseButtonTempeh,
    touch::TouchPhase as TouchPhaseTempeh,
};
use tempeh_window::input::{InputManager, KeyState};
use winit::event::VirtualKeyCode;
use winit::event::{ElementState, MouseButton, TouchPhase, WindowEvent};
use winit::event_loop::ControlFlow;

pub struct InputProcessor {
    pub input_manager: InputManager,
}

impl InputProcessor {
    pub fn new() -> Self {
        Self {
            input_manager: InputManager::new(),
        }
    }

    pub fn reset(&mut self) {
        self.input_manager.reset();
    }

    pub fn handle_input(&mut self, event: &winit::event::WindowEvent) {
        match event {
            WindowEvent::KeyboardInput { input, .. } => {
                let virtual_keycode =
                    input
                        .virtual_keycode
                        .map(|virtual_keycode| match virtual_keycode {
                            VirtualKeyCode::Key1 => VirtualKeyCodeTempeh::Key1,
                            VirtualKeyCode::Key2 => VirtualKeyCodeTempeh::Key2,
                            VirtualKeyCode::Key3 => VirtualKeyCodeTempeh::Key3,
                            VirtualKeyCode::Key4 => VirtualKeyCodeTempeh::Key4,
                            VirtualKeyCode::Key5 => VirtualKeyCodeTempeh::Key5,
                            VirtualKeyCode::Key6 => VirtualKeyCodeTempeh::Key6,
                            VirtualKeyCode::Key7 => VirtualKeyCodeTempeh::Key7,
                            VirtualKeyCode::Key8 => VirtualKeyCodeTempeh::Key8,
                            VirtualKeyCode::Key9 => VirtualKeyCodeTempeh::Key9,
                            VirtualKeyCode::Key0 => VirtualKeyCodeTempeh::Key0,
                            VirtualKeyCode::A => VirtualKeyCodeTempeh::A,
                            VirtualKeyCode::B => VirtualKeyCodeTempeh::B,
                            VirtualKeyCode::C => VirtualKeyCodeTempeh::C,
                            VirtualKeyCode::D => VirtualKeyCodeTempeh::D,
                            VirtualKeyCode::E => VirtualKeyCodeTempeh::E,
                            VirtualKeyCode::F => VirtualKeyCodeTempeh::F,
                            VirtualKeyCode::G => VirtualKeyCodeTempeh::G,
                            VirtualKeyCode::H => VirtualKeyCodeTempeh::H,
                            VirtualKeyCode::I => VirtualKeyCodeTempeh::I,
                            VirtualKeyCode::J => VirtualKeyCodeTempeh::J,
                            VirtualKeyCode::K => VirtualKeyCodeTempeh::K,
                            VirtualKeyCode::L => VirtualKeyCodeTempeh::L,
                            VirtualKeyCode::M => VirtualKeyCodeTempeh::M,
                            VirtualKeyCode::N => VirtualKeyCodeTempeh::N,
                            VirtualKeyCode::O => VirtualKeyCodeTempeh::O,
                            VirtualKeyCode::P => VirtualKeyCodeTempeh::P,
                            VirtualKeyCode::Q => VirtualKeyCodeTempeh::Q,
                            VirtualKeyCode::R => VirtualKeyCodeTempeh::R,
                            VirtualKeyCode::S => VirtualKeyCodeTempeh::S,
                            VirtualKeyCode::T => VirtualKeyCodeTempeh::T,
                            VirtualKeyCode::U => VirtualKeyCodeTempeh::U,
                            VirtualKeyCode::V => VirtualKeyCodeTempeh::V,
                            VirtualKeyCode::W => VirtualKeyCodeTempeh::W,
                            VirtualKeyCode::X => VirtualKeyCodeTempeh::X,
                            VirtualKeyCode::Y => VirtualKeyCodeTempeh::Y,
                            VirtualKeyCode::Z => VirtualKeyCodeTempeh::Z,
                            VirtualKeyCode::Escape => VirtualKeyCodeTempeh::Escape,
                            VirtualKeyCode::F1 => VirtualKeyCodeTempeh::F1,
                            VirtualKeyCode::F2 => VirtualKeyCodeTempeh::F2,
                            VirtualKeyCode::F3 => VirtualKeyCodeTempeh::F3,
                            VirtualKeyCode::F4 => VirtualKeyCodeTempeh::F4,
                            VirtualKeyCode::F5 => VirtualKeyCodeTempeh::F5,
                            VirtualKeyCode::F6 => VirtualKeyCodeTempeh::F6,
                            VirtualKeyCode::F7 => VirtualKeyCodeTempeh::F7,
                            VirtualKeyCode::F8 => VirtualKeyCodeTempeh::F8,
                            VirtualKeyCode::F9 => VirtualKeyCodeTempeh::F9,
                            VirtualKeyCode::F10 => VirtualKeyCodeTempeh::F10,
                            VirtualKeyCode::F11 => VirtualKeyCodeTempeh::F11,
                            VirtualKeyCode::F12 => VirtualKeyCodeTempeh::F12,
                            VirtualKeyCode::F13 => VirtualKeyCodeTempeh::F13,
                            VirtualKeyCode::F14 => VirtualKeyCodeTempeh::F14,
                            VirtualKeyCode::F15 => VirtualKeyCodeTempeh::F15,
                            VirtualKeyCode::F16 => VirtualKeyCodeTempeh::F16,
                            VirtualKeyCode::F17 => VirtualKeyCodeTempeh::F17,
                            VirtualKeyCode::F18 => VirtualKeyCodeTempeh::F18,
                            VirtualKeyCode::F19 => VirtualKeyCodeTempeh::F19,
                            VirtualKeyCode::F20 => VirtualKeyCodeTempeh::F20,
                            VirtualKeyCode::F21 => VirtualKeyCodeTempeh::F21,
                            VirtualKeyCode::F22 => VirtualKeyCodeTempeh::F22,
                            VirtualKeyCode::F23 => VirtualKeyCodeTempeh::F23,
                            VirtualKeyCode::F24 => VirtualKeyCodeTempeh::F24,
                            VirtualKeyCode::Snapshot => VirtualKeyCodeTempeh::Snapshot,
                            VirtualKeyCode::Scroll => VirtualKeyCodeTempeh::Scroll,
                            VirtualKeyCode::Pause => VirtualKeyCodeTempeh::Pause,
                            VirtualKeyCode::Insert => VirtualKeyCodeTempeh::Insert,
                            VirtualKeyCode::Home => VirtualKeyCodeTempeh::Home,
                            VirtualKeyCode::Delete => VirtualKeyCodeTempeh::Delete,
                            VirtualKeyCode::End => VirtualKeyCodeTempeh::End,
                            VirtualKeyCode::PageDown => VirtualKeyCodeTempeh::PageDown,
                            VirtualKeyCode::PageUp => VirtualKeyCodeTempeh::PageUp,
                            VirtualKeyCode::Left => VirtualKeyCodeTempeh::Left,
                            VirtualKeyCode::Up => VirtualKeyCodeTempeh::Up,
                            VirtualKeyCode::Right => VirtualKeyCodeTempeh::Right,
                            VirtualKeyCode::Down => VirtualKeyCodeTempeh::Down,
                            VirtualKeyCode::Back => VirtualKeyCodeTempeh::Back,
                            VirtualKeyCode::Return => VirtualKeyCodeTempeh::Return,
                            VirtualKeyCode::Space => VirtualKeyCodeTempeh::Space,
                            VirtualKeyCode::Compose => VirtualKeyCodeTempeh::Compose,
                            VirtualKeyCode::Caret => VirtualKeyCodeTempeh::Caret,
                            VirtualKeyCode::Numlock => VirtualKeyCodeTempeh::Numlock,
                            VirtualKeyCode::Numpad0 => VirtualKeyCodeTempeh::Numpad0,
                            VirtualKeyCode::Numpad1 => VirtualKeyCodeTempeh::Numpad1,
                            VirtualKeyCode::Numpad2 => VirtualKeyCodeTempeh::Numpad2,
                            VirtualKeyCode::Numpad3 => VirtualKeyCodeTempeh::Numpad3,
                            VirtualKeyCode::Numpad4 => VirtualKeyCodeTempeh::Numpad4,
                            VirtualKeyCode::Numpad5 => VirtualKeyCodeTempeh::Numpad5,
                            VirtualKeyCode::Numpad6 => VirtualKeyCodeTempeh::Numpad6,
                            VirtualKeyCode::Numpad7 => VirtualKeyCodeTempeh::Numpad7,
                            VirtualKeyCode::Numpad8 => VirtualKeyCodeTempeh::Numpad8,
                            VirtualKeyCode::Numpad9 => VirtualKeyCodeTempeh::Numpad9,
                            VirtualKeyCode::AbntC1 => VirtualKeyCodeTempeh::AbntC1,
                            VirtualKeyCode::AbntC2 => VirtualKeyCodeTempeh::AbntC2,
                            VirtualKeyCode::Apostrophe => VirtualKeyCodeTempeh::Apostrophe,
                            VirtualKeyCode::Apps => VirtualKeyCodeTempeh::Apps,
                            VirtualKeyCode::At => VirtualKeyCodeTempeh::At,
                            VirtualKeyCode::Ax => VirtualKeyCodeTempeh::Ax,
                            VirtualKeyCode::Backslash => VirtualKeyCodeTempeh::Backslash,
                            VirtualKeyCode::Calculator => VirtualKeyCodeTempeh::Calculator,
                            VirtualKeyCode::Capital => VirtualKeyCodeTempeh::Capital,
                            VirtualKeyCode::Colon => VirtualKeyCodeTempeh::Colon,
                            VirtualKeyCode::Comma => VirtualKeyCodeTempeh::Comma,
                            VirtualKeyCode::Convert => VirtualKeyCodeTempeh::Convert,
                            VirtualKeyCode::Equals => VirtualKeyCodeTempeh::Equals,
                            VirtualKeyCode::Grave => VirtualKeyCodeTempeh::Grave,
                            VirtualKeyCode::Kana => VirtualKeyCodeTempeh::Kana,
                            VirtualKeyCode::Kanji => VirtualKeyCodeTempeh::Kanji,
                            VirtualKeyCode::LAlt => VirtualKeyCodeTempeh::LAlt,
                            VirtualKeyCode::LBracket => VirtualKeyCodeTempeh::LBracket,
                            VirtualKeyCode::LControl => VirtualKeyCodeTempeh::LControl,
                            VirtualKeyCode::LShift => VirtualKeyCodeTempeh::LShift,
                            VirtualKeyCode::LWin => VirtualKeyCodeTempeh::LWin,
                            VirtualKeyCode::Mail => VirtualKeyCodeTempeh::Mail,
                            VirtualKeyCode::MediaSelect => VirtualKeyCodeTempeh::MediaSelect,
                            VirtualKeyCode::MediaStop => VirtualKeyCodeTempeh::MediaStop,
                            VirtualKeyCode::Minus => VirtualKeyCodeTempeh::Minus,
                            VirtualKeyCode::Mute => VirtualKeyCodeTempeh::Mute,
                            VirtualKeyCode::MyComputer => VirtualKeyCodeTempeh::MyComputer,
                            VirtualKeyCode::NavigateForward => {
                                VirtualKeyCodeTempeh::NavigateForward
                            }
                            VirtualKeyCode::NavigateBackward => {
                                VirtualKeyCodeTempeh::NavigateBackward
                            }
                            VirtualKeyCode::NextTrack => VirtualKeyCodeTempeh::NextTrack,
                            VirtualKeyCode::NoConvert => VirtualKeyCodeTempeh::NoConvert,
                            VirtualKeyCode::NumpadComma => VirtualKeyCodeTempeh::NumpadComma,
                            VirtualKeyCode::NumpadEnter => VirtualKeyCodeTempeh::NumpadEnter,
                            VirtualKeyCode::NumpadEquals => VirtualKeyCodeTempeh::NumpadEquals,
                            VirtualKeyCode::OEM102 => VirtualKeyCodeTempeh::OEM102,
                            VirtualKeyCode::Period => VirtualKeyCodeTempeh::Period,
                            VirtualKeyCode::PlayPause => VirtualKeyCodeTempeh::PlayPause,
                            VirtualKeyCode::Power => VirtualKeyCodeTempeh::Power,
                            VirtualKeyCode::PrevTrack => VirtualKeyCodeTempeh::PrevTrack,
                            VirtualKeyCode::RAlt => VirtualKeyCodeTempeh::RAlt,
                            VirtualKeyCode::RBracket => VirtualKeyCodeTempeh::RBracket,
                            VirtualKeyCode::RControl => VirtualKeyCodeTempeh::RControl,
                            VirtualKeyCode::RShift => VirtualKeyCodeTempeh::RShift,
                            VirtualKeyCode::RWin => VirtualKeyCodeTempeh::RWin,
                            VirtualKeyCode::Semicolon => VirtualKeyCodeTempeh::Semicolon,
                            VirtualKeyCode::Slash => VirtualKeyCodeTempeh::Slash,
                            VirtualKeyCode::Sleep => VirtualKeyCodeTempeh::Sleep,
                            VirtualKeyCode::Stop => VirtualKeyCodeTempeh::Stop,
                            VirtualKeyCode::Sysrq => VirtualKeyCodeTempeh::Sysrq,
                            VirtualKeyCode::Tab => VirtualKeyCodeTempeh::Tab,
                            VirtualKeyCode::Underline => VirtualKeyCodeTempeh::Underline,
                            VirtualKeyCode::Unlabeled => VirtualKeyCodeTempeh::Unlabeled,
                            VirtualKeyCode::VolumeDown => VirtualKeyCodeTempeh::VolumeDown,
                            VirtualKeyCode::VolumeUp => VirtualKeyCodeTempeh::VolumeUp,
                            VirtualKeyCode::Wake => VirtualKeyCodeTempeh::Wake,
                            VirtualKeyCode::WebBack => VirtualKeyCodeTempeh::WebBack,
                            VirtualKeyCode::WebFavorites => VirtualKeyCodeTempeh::WebFavorites,
                            VirtualKeyCode::WebForward => VirtualKeyCodeTempeh::WebForward,
                            VirtualKeyCode::WebHome => VirtualKeyCodeTempeh::WebHome,
                            VirtualKeyCode::WebRefresh => VirtualKeyCodeTempeh::WebRefresh,
                            VirtualKeyCode::WebSearch => VirtualKeyCodeTempeh::WebSearch,
                            VirtualKeyCode::WebStop => VirtualKeyCodeTempeh::WebStop,
                            VirtualKeyCode::Yen => VirtualKeyCodeTempeh::Yen,
                            VirtualKeyCode::Copy => VirtualKeyCodeTempeh::Copy,
                            VirtualKeyCode::Paste => VirtualKeyCodeTempeh::Paste,
                            VirtualKeyCode::Cut => VirtualKeyCodeTempeh::Cut,
                            VirtualKeyCode::NumpadAdd => VirtualKeyCodeTempeh::NumpadAdd,
                            VirtualKeyCode::NumpadDecimal => VirtualKeyCodeTempeh::NumpadDecimal,
                            VirtualKeyCode::NumpadDivide => VirtualKeyCodeTempeh::NumpadDivide,
                            VirtualKeyCode::NumpadMultiply => VirtualKeyCodeTempeh::NumpadMultiply,
                            VirtualKeyCode::NumpadSubtract => VirtualKeyCodeTempeh::NumpadSubtract,
                            VirtualKeyCode::Asterisk => VirtualKeyCodeTempeh::Asterisk,
                            VirtualKeyCode::Plus => VirtualKeyCodeTempeh::Plus,
                        });
                log::info!("Keypress {:?}", input);
                self.input_manager.keyboard_presses.insert(KeyboardInput {
                    state: match input.state {
                        ElementState::Pressed => KeyState::Pressed,
                        ElementState::Released => KeyState::Released,
                    },
                    scancode: input.scancode,
                    virtual_keycode,
                });
            }
            WindowEvent::MouseInput { button, state, .. } => {
                // engine.resources.insert(Input::MouseInput {
                //     state: match state {
                //         ElementState::Pressed => KeyState::Pressed,
                //         ElementState::Released => KeyState::Released,
                //     },
                //     button: match button {
                //         MouseButton::Left => MouseButtonTempeh::Left,
                //         MouseButton::Right => MouseButtonTempeh::Right,
                //         MouseButton::Middle => MouseButtonTempeh::Middle,
                //         MouseButton::Other(x) => MouseButtonTempeh::Other(x),
                //     },
                // });
                log::info!("Mouse input button {:?} state {:?}", button, state);
            }
            WindowEvent::Touch(touch) => {
                // engine.resources.insert(Input::TouchInput {
                //     id: touch.id,
                //     logical_position: tempeh_math::Point2::<f64>::new(
                //         touch.location.x,
                //         touch.location.y,
                //     ),
                //     phase: match touch.phase {
                //         TouchPhase::Started => TouchPhaseTempeh::Started,
                //         TouchPhase::Moved => TouchPhaseTempeh::Moved,
                //         TouchPhase::Ended => TouchPhaseTempeh::Ended,
                //         TouchPhase::Cancelled => TouchPhaseTempeh::Cancelled,
                //     },
                // });
                log::info!("Touch on {:?}", touch.location);
            }
            _ => {}
        }
    }
}
