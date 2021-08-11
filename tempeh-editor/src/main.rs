mod file;
mod ui;

use crate::file::open_file;
use crate::ui::filetree::{imgui_file_tree, FileTree};
use imgui::*;
use imgui_wgpu::{Renderer, RendererConfig};
use pollster::block_on;
use simplelog::*;
use simplelog::{CombinedLogger, Config, LevelFilter, WriteLogger};
use std::io::Write;
use std::path::{Path, PathBuf};
use std::rc::Rc;
use std::sync::{Arc, Mutex};
use std::time::Instant;
use winit::monitor::VideoMode;
use winit::window::Fullscreen;
use winit::{
    dpi::LogicalSize,
    event::{ElementState, Event, KeyboardInput, VirtualKeyCode, WindowEvent},
    event_loop::{ControlFlow, EventLoop},
    window::{Window, WindowBuilder},
};

fn main() {
    // env_logger::init();
    let mut log_messages = Arc::new(Mutex::new(Vec::new()));
    let mut l = Arc::clone(&log_messages);
    env_logger::builder()
        .format(move |buf, record| {
            l.lock()
                .unwrap()
                .push(format!("{}: {}", record.level(), record.args()));
            writeln!(buf, "{}: {}", record.level(), record.args())
        })
        .init();
    // open_file(PathBuf::from(
    //     "C:\\Users\\andra\\Projects\\color-joy\\canvas_fill.gd",
    // ));
    let file_tree = FileTree::from(PathBuf::from(
        "C:\\Users\\andra\\Projects\\wgpu-playground\\tempeh-editor",
    ));
    // CombinedLogger::init(vec![
    //     TermLogger::new(
    //         LevelFilter::Warn,
    //         Config::default(),
    //         TerminalMode::Mixed,
    //         ColorChoice::Auto,
    //     ),
    //     WriteLogger::new(
    //         LevelFilter::Info,
    //         Config::default(),
    //         ,
    //     ),
    // ])
    // .unwrap();

    // Set up window and GPU
    let event_loop = EventLoop::new();

    let instance = wgpu::Instance::new(wgpu::BackendBit::PRIMARY);

    let (window, size, surface) = {
        let window = WindowBuilder::new()
            .with_title("Tempeh Editor")
            .with_inner_size(LogicalSize {
                width: 1280.0,
                height: 720.0,
            })
            .build(&event_loop)
            .unwrap();
        let size = window.inner_size();

        let surface = unsafe { instance.create_surface(&window) };

        (window, size, surface)
    };

    let hidpi_factor = window.scale_factor();

    let adapter = block_on(instance.request_adapter(&wgpu::RequestAdapterOptions {
        power_preference: wgpu::PowerPreference::HighPerformance,
        compatible_surface: Some(&surface),
    }))
    .unwrap();

    let (device, queue) =
        block_on(adapter.request_device(&wgpu::DeviceDescriptor::default(), None)).unwrap();

    // Set up swap chain
    let sc_desc = wgpu::SwapChainDescriptor {
        usage: wgpu::TextureUsage::RENDER_ATTACHMENT,
        format: wgpu::TextureFormat::Bgra8UnormSrgb,
        width: size.width as u32,
        height: size.height as u32,
        present_mode: wgpu::PresentMode::Mailbox,
    };

    let mut swap_chain = device.create_swap_chain(&surface, &sc_desc);

    // Set up dear imgui
    let mut imgui = imgui::Context::create();

    let mut platform = imgui_winit_support::WinitPlatform::init(&mut imgui);
    platform.attach_window(
        imgui.io_mut(),
        &window,
        imgui_winit_support::HiDpiMode::Default,
    );
    imgui.set_ini_filename(None);

    let font_size = (13.0 * hidpi_factor) as f32;
    imgui.io_mut().font_global_scale = (1.0 / hidpi_factor) as f32;

    imgui.fonts().add_font(&[FontSource::DefaultFontData {
        config: Some(imgui::FontConfig {
            oversample_h: 1,
            pixel_snap_h: true,
            size_pixels: font_size,
            ..Default::default()
        }),
    }]);

    //
    // Set up dear imgui wgpu renderer
    //
    let clear_color = wgpu::Color {
        r: 0.1,
        g: 0.2,
        b: 0.3,
        a: 1.0,
    };

    let renderer_config = RendererConfig {
        texture_format: sc_desc.format,
        ..Default::default()
    };

    let mut renderer = Renderer::new(&mut imgui, &device, &queue, renderer_config);

    let mut last_frame = Instant::now();

    let mut last_cursor = None;

    // Event loop
    event_loop.run(move |event, _, control_flow| {
        *control_flow = if cfg!(feature = "metal-auto-capture") {
            ControlFlow::Exit
        } else {
            ControlFlow::Poll
        };
        match event {
            Event::WindowEvent {
                event: WindowEvent::Resized(_),
                ..
            } => {
                let size = window.inner_size();

                let sc_desc = wgpu::SwapChainDescriptor {
                    usage: wgpu::TextureUsage::RENDER_ATTACHMENT,
                    format: wgpu::TextureFormat::Bgra8UnormSrgb,
                    width: size.width as u32,
                    height: size.height as u32,
                    present_mode: wgpu::PresentMode::Mailbox,
                };

                swap_chain = device.create_swap_chain(&surface, &sc_desc);
            }
            Event::WindowEvent {
                event:
                    WindowEvent::KeyboardInput {
                        input:
                            KeyboardInput {
                                virtual_keycode: Some(VirtualKeyCode::Escape),
                                state: ElementState::Pressed,
                                ..
                            },
                        ..
                    },
                ..
            }
            | Event::WindowEvent {
                event: WindowEvent::CloseRequested,
                ..
            } => {
                *control_flow = ControlFlow::Exit;
            }
            Event::MainEventsCleared => window.request_redraw(),
            Event::RedrawEventsCleared => {
                let delta_s = last_frame.elapsed();
                let now = Instant::now();
                imgui.io_mut().update_delta_time(now - last_frame);
                last_frame = now;

                let frame = match swap_chain.get_current_frame() {
                    Ok(frame) => frame,
                    Err(e) => {
                        eprintln!("dropped frame: {:?}", e);
                        return;
                    }
                };
                platform
                    .prepare_frame(imgui.io_mut(), &window)
                    .expect("Failed to prepare frame");
                let ui = imgui.frame();

                {
                    if let Some(menu_bar) = ui.begin_main_menu_bar() {
                        if let Some(menu) = ui.begin_menu(im_str!("Project"), true) {
                            MenuItem::new(im_str!("Main menu bar")).build(&ui);
                            MenuItem::new(im_str!("Quit to Project Menu")).build(&ui);
                            menu.end(&ui);
                        }
                        if let Some(menu) = ui.begin_menu(im_str!("Help"), true) {
                            MenuItem::new(im_str!("About")).build(&ui);
                            MenuItem::new(im_str!("Repository")).build(&ui);
                            menu.end(&ui);
                        }
                        menu_bar.end(&ui);
                    }

                    let window = imgui::Window::new(im_str!("a"));
                    window
                        .size([300.0, 500.0], Condition::FirstUseEver)
                        .position([0.0, 16.0], Condition::FirstUseEver)
                        .menu_bar(true)
                        .build(&ui, || {
                            if let Some(listbox) = ListBox::new(im_str!("Entities")).begin(&ui) {
                                for i in 0..10 {
                                    if Selectable::new(im_str!("test")).selected(i == 8).build(&ui)
                                    {
                                    }
                                }
                                listbox.end(&ui);
                            }

                            ui.separator();

                            ui.text(im_str!("Hello world!"));
                            ui.text(im_str!("This...is...imgui-rs on WGPU!"));
                            ui.separator();
                            let mouse_pos = ui.io().mouse_pos;
                            ui.text(im_str!(
                                "Mouse Position: ({:.1},{:.1})",
                                mouse_pos[0],
                                mouse_pos[1]
                            ));
                            ui.separator();

                            let mut path = PathBuf::new();
                            imgui_file_tree(&ui, &file_tree, &mut path);
                        });

                    let window = imgui::Window::new(im_str!("b"));
                    window
                        .size([300.0, 500.0], Condition::FirstUseEver)
                        .position([700.0, 16.0], Condition::FirstUseEver)
                        .menu_bar(true)
                        .build(&ui, || {
                            let mut name = imgui::ImString::from(String::from("sample"));
                            ui.input_text(im_str!("Name"), &mut name);

                            let mut arr2 = [0f32; 2];
                            let mut arr3 = [0f32; 3];
                            ui.input_float2(im_str!("Position"), &mut arr2).build();
                            ui.input_float3(im_str!("Rotation"), &mut arr3).build();
                            ui.input_float3(im_str!("Scale"), &mut arr3).build();
                        });

                    let window = imgui::Window::new(im_str!("c"));
                    window
                        .size([1000.0, 300.0], Condition::FirstUseEver)
                        .position([0.0, 500.0], Condition::FirstUseEver)
                        .menu_bar(true)
                        .build(&ui, || {
                            TabBar::new(im_str!("Bar")).build(&ui, || {
                                let mut open_storage = true;
                                TabItem::new(im_str!("Storage"))
                                    .opened(&mut open_storage)
                                    .build(&ui, || {
                                        if let Some(listbox) =
                                            ListBox::new(im_str!("Storage")).begin(&ui)
                                        {
                                            // filetree();
                                            listbox.end(&ui);
                                        }
                                    });

                                let mut open_log = true;
                                TabItem::new(im_str!("Log")).opened(&mut open_log).build(
                                    &ui,
                                    || {
                                        if let Some(listbox) =
                                            ListBox::new(im_str!("Log")).begin(&ui)
                                        {
                                            for message in log_messages.lock().unwrap().iter().rev()
                                            {
                                                ui.text(message);
                                            }
                                            listbox.end(&ui);
                                        }
                                    },
                                );
                            });
                        });
                }

                let mut encoder: wgpu::CommandEncoder =
                    device.create_command_encoder(&wgpu::CommandEncoderDescriptor { label: None });

                if last_cursor != Some(ui.mouse_cursor()) {
                    last_cursor = Some(ui.mouse_cursor());
                    platform.prepare_render(&ui, &window);
                }

                let mut rpass = encoder.begin_render_pass(&wgpu::RenderPassDescriptor {
                    label: None,
                    color_attachments: &[wgpu::RenderPassColorAttachment {
                        view: &frame.output.view,
                        resolve_target: None,
                        ops: wgpu::Operations {
                            load: wgpu::LoadOp::Clear(clear_color),
                            store: true,
                        },
                    }],
                    depth_stencil_attachment: None,
                });

                renderer
                    .render(ui.render(), &queue, &device, &mut rpass)
                    .expect("Rendering failed");

                drop(rpass);

                queue.submit(Some(encoder.finish()));
            }
            _ => (),
        }

        platform.handle_event(imgui.io_mut(), &window, &event);
    });
}
