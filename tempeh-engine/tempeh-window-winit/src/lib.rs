mod event;
mod mini_block;

use crate::event::InputProcessor;
use instant::{Duration, Instant};
use log::info;
use mini_block::BlockingFuture;
use std::borrow::{Borrow, BorrowMut};
use std::cell::RefCell;
use std::ffi::{CStr, CString};
use std::io::{BufReader, Cursor, Read};
use std::rc::Rc;
use std::sync::{Arc, Mutex};
use tempeh_ecs::{Resources, Schedule, World};
use tempeh_engine::Engine;
use tempeh_renderer::renderer::Renderer;
use tempeh_renderer::state::State;
use tempeh_window::input::touch::TouchPhase as TouchPhaseTempeh;
use tempeh_window::input::{mouse::MouseButton as MouseButtonTempeh, InputManager, KeyState};
use tempeh_window::{HasRawWindowHandle, Runner, ScreenSize, TempehWindow};
use wgpu::{Color, SwapChainError};
use winit::dpi::PhysicalSize;
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

    fn get_window_size(&self) -> ScreenSize {
        let PhysicalSize { width, height } = self.window.inner_size();
        ScreenSize { width, height }
    }

    type Window = Window;
    fn get_raw_window_handle(&self) -> &Self::Window {
        &self.window
    }
}

impl Runner for WinitWindow {
    fn run(mut self, mut engine: Engine) {
        let mut size = self.window.inner_size();
        if cfg!(not(target_os = "android")) {
            engine
                .resources
                .insert(tempeh_renderer::renderer::Renderer::new(
                    &self.window,
                    ScreenSize {
                        width: size.width,
                        height: size.height,
                    },
                    Color {
                        r: 0.8,
                        g: 0.8,
                        b: 0.8,
                        a: 1.0,
                    },
                ));
        }

        let mut input_processor = InputProcessor::new();
        let mut time = Instant::now();
        self.event_loop.take().unwrap().run(
            move |event, _even_loop_window_target, control_flow| {
                *control_flow = ControlFlow::Poll;
                time = Instant::now();

                match event {
                    Event::Resumed => {
                        if cfg!(target_os = "android") {
                            size = self.window.inner_size();
                            engine
                                .resources
                                .insert(tempeh_renderer::renderer::Renderer::new(
                                    &self.window,
                                    ScreenSize {
                                        width: size.width,
                                        height: size.height,
                                    },
                                    Color {
                                        r: 0.8,
                                        g: 0.8,
                                        b: 0.8,
                                        a: 1.0,
                                    },
                                ));
                        }
                    }
                    Event::WindowEvent { event, window_id } if window_id == self.window.id() => {
                        match event {
                            WindowEvent::CloseRequested => {
                                *control_flow = ControlFlow::Exit;
                            }
                            WindowEvent::Resized(size_) => {
                                size = size_;
                                engine.resources.get_mut::<Renderer>().unwrap().resize(
                                    ScreenSize {
                                        width: size.width,
                                        height: size.height,
                                    },
                                );
                            }
                            _ => {}
                        };
                        input_processor.handle_input(&event);
                    }
                    Event::RedrawRequested(window_id) => {
                        engine
                            .resources
                            .insert(input_processor.input_manager.clone());
                        engine.resources.insert(time.elapsed());
                        engine
                            .schedule
                            .execute(&mut engine.world, &mut engine.resources);
                        input_processor.reset();
                        // match engine.resources.get::<Renderer>().unwrap().render() {
                        //     Ok(_) => {}
                        //     Err(SwapChainError::Lost) => engine
                        //         .resources
                        //         .get_mut::<Renderer>()
                        //         .unwrap()
                        //         .resize(ScreenSize {
                        //             width: size.width,
                        //             height: size.height,
                        //         }),
                        //     Err(SwapChainError::Outdated) => eprintln!("Swapchain outdated"),
                        //     Err(SwapChainError::OutOfMemory) => {
                        //         eprintln!("Swapchain out of memory")
                        //     }
                        //     Err(SwapChainError::Timeout) => eprintln!("Swapchain timeout"),
                        // }
                    }
                    Event::MainEventsCleared => {
                        self.window.request_redraw();
                    }
                    _ => {}
                };
            },
        );
    }
}
