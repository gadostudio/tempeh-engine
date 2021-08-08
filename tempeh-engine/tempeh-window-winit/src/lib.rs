use async_std::task::block_on;
use legion::{Resources, Schedule, World};
use log::info;
use std::cell::RefCell;
use std::ffi::{CStr, CString};
use std::io::{BufReader, Cursor, Read};
use std::rc::Rc;
use std::sync::{Arc, Mutex};
use std::time::{Duration, Instant};
use tempeh_core::game::Engine;
use tempeh_renderer;
use tempeh_renderer::{Renderer, ScreenSize};
use tempeh_window::input::touch::TouchPhase as TouchPhaseTempeh;
use tempeh_window::input::{mouse::MouseButton as MouseButtonTempeh, Input, KeyState};
use tempeh_window::{Runner, TempehWindow};
use wgpu::SwapChainError;
use winit::event::{TouchPhase, VirtualKeyCode};
use winit::window::{UserAttentionType, Window};
use winit::{
    event::ElementState,
    event::{Event, MouseButton, WindowEvent},
    event_loop::{ControlFlow, EventLoop},
    window::WindowBuilder,
};

pub struct WinitWindow {
    event_loop: Option<EventLoop<()>>,
    window: Window,
}

impl WinitWindow {
    pub fn new() -> Self {
        let event_loop = EventLoop::new();
        let window = WindowBuilder::new()
            .with_title("2048")
            .build(&event_loop)
            .unwrap();
        #[cfg(target_arch = "wasm32")]
        {
            use winit::platform::web::WindowExtWebSys;

            let canvas = window.canvas();

            let window = web_sys::window().unwrap();
            let document = window.document().unwrap();
            let body = document.body().unwrap();

            body.append_child(&canvas)
                .expect("Append canvas to HTML body");
        }

        Self {
            window,
            event_loop: Some(event_loop),
        }
    }
}

impl TempehWindow for WinitWindow {
    fn focus(&mut self) {
        self.window
            .request_user_attention(Some(UserAttentionType::Critical));
    }

    fn set_title(&mut self, title: &str) {
        self.window.set_title(title);
    }
}

impl Runner for WinitWindow {
    fn run(mut self, mut engine: Engine) {
        let mut size = self.window.inner_size();
        let mut renderer: Option<tempeh_renderer::Renderer> = None;
        if cfg!(not(target_os = "android")) {
            renderer = Some(block_on(tempeh_renderer::Renderer::new(
                &self.window,
                ScreenSize {
                    width: size.width,
                    height: size.height,
                },
            )));
        }

        let mut time = Instant::now();
        self.event_loop
            .take()
            .unwrap()
            .run(move |event, even_loop_window_target, control_flow| {
                *control_flow = ControlFlow::Poll;
                time = Instant::now();

                match event {
                    Event::Resumed => {
                        if cfg!(target_os = "android") {
                            size = self.window.inner_size();
                            renderer = Some(block_on(tempeh_renderer::Renderer::new(
                                &self.window,
                                ScreenSize {
                                    width: size.width,
                                    height: size.height,
                                },
                            )));
                        }
                    }
                    Event::WindowEvent { event, window_id } if window_id == self.window.id() => {
                        match event {
                            WindowEvent::CloseRequested => {
                                *control_flow = ControlFlow::Exit;
                            }
                            WindowEvent::Resized(size_) => {
                                size = size_;
                                renderer.as_mut().unwrap().resize(ScreenSize {
                                    width: size.width,
                                    height: size.height,
                                });
                            }
                            WindowEvent::KeyboardInput { input, .. } => {
                                log::info!(
                                    "Keyboard key {:?} state {:?}",
                                    input.virtual_keycode.unwrap_or(VirtualKeyCode::Escape),
                                    input.state
                                );
                            }
                            WindowEvent::MouseInput { button, state, .. } => {
                                engine.resources.insert(Input::MouseInput {
                                    state: match state {
                                        ElementState::Pressed => KeyState::Pressed,
                                        ElementState::Released => KeyState::Released,
                                    },
                                    button: match button {
                                        MouseButton::Left => MouseButtonTempeh::Left,
                                        MouseButton::Right => MouseButtonTempeh::Right,
                                        MouseButton::Middle => MouseButtonTempeh::Middle,
                                        MouseButton::Other(x) => MouseButtonTempeh::Other(x),
                                    },
                                });
                                log::info!("Mouse input button {:?} state {:?}", button, state);
                            }
                            WindowEvent::Touch(touch) => {
                                engine.resources.insert(Input::TouchInput {
                                    id: touch.id,
                                    logical_position: nalgebra::Point2::<f64>::new(
                                        touch.location.x,
                                        touch.location.y,
                                    ),
                                    phase: match touch.phase {
                                        TouchPhase::Started => TouchPhaseTempeh::Started,
                                        TouchPhase::Moved => TouchPhaseTempeh::Moved,
                                        TouchPhase::Ended => TouchPhaseTempeh::Ended,
                                        TouchPhase::Cancelled => TouchPhaseTempeh::Cancelled,
                                    },
                                });
                                log::info!("Touch on {:?}", touch.location);
                            }
                            _ => {}
                        }
                    }
                    Event::RedrawRequested(window_id) => {
                        match renderer.as_ref().unwrap().render() {
                            Ok(_) => {}
                            Err(SwapChainError::Lost) => {
                                renderer.as_mut().unwrap().resize(ScreenSize {
                                    width: size.width,
                                    height: size.height,
                                })
                            }
                            Err(SwapChainError::Outdated) => eprintln!("Swapchain outdated"),
                            Err(SwapChainError::OutOfMemory) => {
                                eprintln!("Swapchain out of memory")
                            }
                            Err(SwapChainError::Timeout) => eprintln!("Swapchain timeout"),
                        }
                    }
                    Event::MainEventsCleared => {
                        self.window.request_redraw();
                    }
                    _ => {}
                };
                engine.resources.insert(Instant::now().duration_since(time));
                engine
                    .schedule
                    .execute(&mut engine.world, &mut engine.resources);
            });
    }
}
