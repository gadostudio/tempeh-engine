mod core;
mod ecs;
mod renderer;

use async_std::task::block_on;
use log::info;
use renderer::Renderer;
use std::ffi::{CStr, CString};
use std::io::{BufReader, Cursor, Read};
use std::sync::{Arc, Mutex};
use wgpu::{Color, SwapChainError};
use winit::{
    event::{Event, WindowEvent},
    event_loop::{ControlFlow, EventLoop},
    window::WindowBuilder,
};

pub fn main() {
    // #[cfg(target_os = "android")]
    // {
    //     let native_activity = ndk_glue::native_activity();
    //     let asset_manager = native_activity.asset_manager();
    //     let asset = asset_manager
    //         .open(&CString::new("imperial.ogg").unwrap())
    //         .unwrap();
    //     let mut data = Vec::with_capacity(asset.get_length());
    //     BufReader::new(asset).read_to_end(&mut data).unwrap();
    //     let (_stream, stream_handle) = rodio::OutputStream::try_default().unwrap();
    //     stream_handle.play_once(Cursor::new(data));
    // }

    let event_loop = EventLoop::new();
    let window = WindowBuilder::new()
        .with_title("WGPU Playground")
        .build(&event_loop)
        .unwrap();

    let mut renderer: Option<Renderer> = None;
    if cfg!(not(target_os = "android")) {
        renderer = Some(block_on(Renderer::new(&window)));
    }

    event_loop.run(move |event, even_loop_window_target, control_flow| {
        *control_flow = ControlFlow::Poll;

        match event {
            Event::Resumed => {
                if cfg!(target_os = "android") {
                    renderer = Some(block_on(Renderer::new(&window)));
                }
            }
            Event::WindowEvent { event, window_id } if window_id == window.id() => match event {
                WindowEvent::CloseRequested => {
                    *control_flow = ControlFlow::Exit;
                }
                WindowEvent::Resized(size) => {
                    renderer.as_mut().unwrap().resize(size);
                }
                WindowEvent::KeyboardInput { input, .. } => {
                    println!("Scancode {}", input.scancode);
                }
                _ => {}
            },
            Event::RedrawRequested(window_id) => match renderer.as_ref().unwrap().render() {
                Ok(_) => {}
                Err(SwapChainError::Lost) => renderer.as_mut().unwrap().resize(window.inner_size()),
                Err(SwapChainError::Outdated) => eprintln!("Swapchain outdated"),
                Err(SwapChainError::OutOfMemory) => eprintln!("Swapchain out of memory"),
                Err(SwapChainError::Timeout) => eprintln!("Swapchain timeout"),
            },
            Event::MainEventsCleared => {
                window.request_redraw();
            }
            _ => {}
        };
    });
}
