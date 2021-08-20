mod event;
mod mini_block;

use crate::event::InputProcessor;
use instant::{Duration, Instant};
use log::info;
use mini_block::BlockingFuture;
use std::cell::RefCell;
use std::ffi::{CStr, CString};
use std::io::{BufReader, Cursor, Read};
use std::rc::Rc;
use std::sync::{Arc, Mutex};
use tempeh_core::app::Engine;
use tempeh_ecs::{Resources, Schedule, World};
use tempeh_renderer::state::State;
use tempeh_renderer::ScreenSize;
use tempeh_window::input::touch::TouchPhase as TouchPhaseTempeh;
use tempeh_window::input::{mouse::MouseButton as MouseButtonTempeh, InputManager, KeyState};
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
        let mut renderer: Option<tempeh_renderer::state::State> = None;
        if cfg!(not(target_os = "android")) {
            renderer = Some(
                tempeh_renderer::state::State::new(
                    &self.window,
                    ScreenSize {
                        width: size.width,
                        height: size.height,
                    },
                )
                .block(),
            );
        }

        let mut input_processor = InputProcessor::new();
        let mut time = Instant::now();
        self.event_loop.take().unwrap().run(
            move |event, _even_loop_window_target, control_flow| {
                *control_flow = ControlFlow::Poll;
                time = Instant::now();
                input_processor.reset();

                match event {
                    Event::Resumed => {
                        if cfg!(target_os = "android") {
                            size = self.window.inner_size();
                            renderer = Some(
                                tempeh_renderer::state::State::new(
                                    &self.window,
                                    ScreenSize {
                                        width: size.width,
                                        height: size.height,
                                    },
                                )
                                .block(),
                            );
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
                            _ => {}
                        };
                        input_processor.handle_input(&event);
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

                engine
                    .resources
                    .insert(input_processor.input_manager.clone());
                engine.resources.insert(time.elapsed());
                engine
                    .schedule
                    .execute(&mut engine.world, &mut engine.resources);
            },
        );
    }
}
