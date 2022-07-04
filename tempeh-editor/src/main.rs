mod app;
mod file;
mod logger;
mod message;
mod misc;
mod project;
mod renderer;
mod scene;
mod toolchain;
mod trace;
mod ui;

use crate::app::{App, AppState};
use crate::file::open_file;
use crate::message::Message;
use crate::misc::clipboard::Clipboard;
use crate::project::Project;
use crate::renderer::{Renderer, SceneEditorRendererPipeline, WindowSize};
use crate::scene::run_game;
use crate::toolchain::Toolchain;
use crate::ui::filetree::{imgui_file_tree, FileTree};
use async_std::channel;
use async_std::prelude::*;
use async_std::task;
use imgui::*;
use pollster::block_on;
use simplelog::*;
use simplelog::{CombinedLogger, Config, LevelFilter, WriteLogger};
use std::io::Write;
use std::path::{Path, PathBuf};
use std::rc::Rc;
use std::sync::{Arc, Mutex};
use std::time::Instant;
use wgpu::TextureViewDescriptor;
use winit::monitor::VideoMode;
use winit::window::Fullscreen;
use winit::{
    dpi::LogicalSize,
    event::{ElementState, Event, KeyboardInput, VirtualKeyCode, WindowEvent},
    event_loop::{ControlFlow, EventLoop},
    window::{Window, WindowBuilder},
};

const REPOSITORY_URL: &str = "https://github.com/shaderboi/tempeh-engine";

#[cfg(windows)]
const LINE_ENDING: &'static str = "\r\n";
#[cfg(not(windows))]
const LINE_ENDING: &'static str = "\n";

#[async_std::main]
async fn main() {
    logger::init();
    let mut project = Project::new();
    let project_path =
        PathBuf::from("C:\\Users\\andra\\Projects\\tempeh-engine\\examples\\hello-world");
    project.open(&project_path);
    let file_tree = FileTree::from(&project_path);

    let toolchain = Arc::new(Toolchain::from().await);

    // Set up window and GPU
    let event_loop = EventLoop::new();

    let (window, mut physical_size) = {
        let window = WindowBuilder::new()
            .with_title("Tempeh Editor")
            .with_inner_size(LogicalSize {
                width: 1280.0,
                height: 720.0,
            })
            .build(&event_loop)
            .unwrap();
        let size = window.inner_size();

        (window, size)
    };

    let hidpi_factor = window.scale_factor();

    // Set up swap
    // let sc_desc = wgpu::SwapChainDescriptor {
    //     usage: wgpu::TextureUsage::RENDER_ATTACHMENT,
    //     format: wgpu::TextureFormat::Bgra8UnormSrgb,
    //     width: physical_size.width as u32,
    //     height: physical_size.height as u32,
    //     present_mode: wgpu::PresentMode::Mailbox,
    // };

    // let mut swap_chain = device.create_swap_chain(&surface, &sc_desc);

    // Set up dear imgui
    let mut imgui_context = imgui::Context::create();

    let mut platform = imgui_winit_support::WinitPlatform::init(&mut imgui_context);
    platform.attach_window(
        imgui_context.io_mut(),
        &window,
        imgui_winit_support::HiDpiMode::Default,
    );
    imgui_context.set_clipboard_backend(Clipboard::new().unwrap());
    imgui_context.set_ini_filename(None);
    {
        let mut style = imgui_context.style_mut();
        style.use_light_colors();
    }

    let font_size = (13.0 * hidpi_factor) as f32;
    imgui_context.io_mut().font_global_scale = (1.0 / hidpi_factor) as f32;

    imgui_context
        .fonts()
        .add_font(&[FontSource::DefaultFontData {
            config: Some(imgui::FontConfig {
                oversample_h: 1,
                pixel_snap_h: true,
                size_pixels: font_size,
                ..Default::default()
            }),
        }]);

    let mut renderer = Renderer::new(&window, &mut imgui_context, &physical_size);

    // let tex = Texture::new(
    //     &device,
    //     &renderer,
    //     TextureConfig {
    //         format: Some(sc_desc.format),
    //         label: Some("scene"),
    //         size: Extent3d {
    //             height: 300,
    //             width: 300,
    //             depth_or_array_layers: 1,
    //         },
    //         dimension: TextureDimension::D2,
    //         mip_level_count: 1,
    //         sample_count: 1,
    //         usage: TextureUsage::COPY_DST | TextureUsage::COPY_SRC | TextureUsage::SAMPLED,
    //     },
    // );
    // let mut d = Vec::new();
    // for i in 0..90000 {
    //     d.push(u8::MIN);
    //     d.push(u8::MIN);
    //     d.push(u8::MAX);
    //     d.push(u8::MAX);
    // }
    // queue.write_texture(
    //     ImageCopyTexture {
    //         texture: &tex.texture(),
    //         mip_level: 0,
    //         origin: wgpu::Origin3d::ZERO,
    //     },
    //     &d,
    //     ImageDataLayout {
    //         offset: 0,
    //         bytes_per_row: Some(std::num::NonZeroU32::new(300 * 4).unwrap()),
    //         rows_per_image: Some(std::num::NonZeroU32::new(300).unwrap()),
    //     },
    //     Extent3d {
    //         height: 300,
    //         width: 300,
    //         depth_or_array_layers: 1,
    //     },
    // );
    // let tex_id = renderer.textures.insert(tex);
    let scene_editor = SceneEditorRendererPipeline::new(
        &mut renderer,
        &WindowSize {
            width: 1,
            height: 1,
        },
    );
    let mut scene_editor_size = [0f32, 0f32];

    let mut last_frame = Instant::now();

    let mut last_cursor = None;
    let (sender, receiver) = channel::unbounded::<String>();
    let sender = Arc::new(sender);

    let mut app = App::new();

    // Event loop
    event_loop.run(move |event, _window_target, control_flow| {
        *control_flow = ControlFlow::Poll;

        for message in app.message_queue.iter() {
            match message {
                Message::Play => {
                    app.ui_state.play_log.clear();
                    let toolchain_ = Arc::clone(&toolchain);
                    let mut sender_ = Arc::clone(&sender);
                    task::spawn(async move {
                        toolchain_
                            .compile_and_record(
                                &sender_,
                                "C:\\Users\\andra\\Projects\\tempeh-engine\\examples\\hello-world",
                            )
                            .await;
                        println!("Done");
                        run_game();
                    });
                }
                Message::ClearLogger => {
                    let mut logger = logger::LOG_MESSAGES.lock().unwrap();
                    logger.clear();
                }
                Message::QuiToProjectSelection => app.state = AppState::ProjectSelection,
                Message::ProjectSettings => {}
                Message::ProjectExport => {
                    app.ui_state.display_export_setup_window = true;
                }
                Message::Settings => {}
            }
        }
        app.message_queue.clear();

        match receiver.try_recv() {
            Ok(line) => {
                app.ui_state.play_log.push_str(&line);
                app.ui_state.play_log.push('\n');
            }
            Err(_) => {}
        };

        match event {
            Event::WindowEvent {
                event: WindowEvent::Resized(size_),
                ..
            } => {
                physical_size = size_;
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
                // let delta_s = last_frame.elapsed();
                let now = Instant::now();
                imgui_context.io_mut().update_delta_time(now - last_frame);
                last_frame = now;

                let frame = renderer.surface.get_current_texture().unwrap();
                platform
                    .prepare_frame(imgui_context.io_mut(), &window)
                    .expect("Failed to prepare frame");
                let imgui_ui = imgui_context.frame();

                match app.state {
                    AppState::ProjectSelection => {}
                    AppState::Workspace => {
                        let mut logger = logger::LOG_MESSAGES.lock().unwrap();
                        let mut log_messages = logger.iter().fold(String::new(), |mut acc, val| {
                            if val
                                .message
                                .to_lowercase()
                                .contains(&app.ui_state.log_filter)
                            {
                                acc.push('[');
                                acc.push_str(&val.target);
                                acc.push_str("] ");
                                acc.push_str(&val.message);
                                acc.push('\n');
                            }
                            acc
                        });

                        {
                            if let Some(menu_bar) = imgui_ui.begin_main_menu_bar() {
                                if let Some(menu) = imgui_ui.begin_menu("File") {
                                    let mut project_settings = false;
                                    MenuItem::new(im_str!("Project Settings"))
                                        .build_with_ref(&imgui_ui, &mut project_settings);
                                    if project_settings {
                                        app.add_message(Message::ProjectSettings);
                                    }
                                    imgui_ui.separator();
                                    let mut build = false;
                                    MenuItem::new(im_str!("Export"))
                                        .build_with_ref(&imgui_ui, &mut build);
                                    if build {
                                        app.add_message(Message::ProjectExport);
                                    }
                                    let mut settings = false;
                                    MenuItem::new(im_str!("Settings"))
                                        .build_with_ref(&imgui_ui, &mut settings);
                                    if settings {
                                        app.add_message(Message::Settings);
                                    }
                                    imgui_ui.separator();
                                    let mut quit_to_project_menu = false;
                                    MenuItem::new(im_str!("Quit to Project Selection"))
                                        .build_with_ref(&imgui_ui, &mut quit_to_project_menu);
                                    if quit_to_project_menu {
                                        app.add_message(Message::QuiToProjectSelection);
                                    }
                                    menu.end();
                                }
                                if let Some(menu) = imgui_ui.begin_menu("Help") {
                                    MenuItem::new(im_str!("About")).build(&imgui_ui);
                                    let mut is_select_repository = false;
                                    MenuItem::new(im_str!("Repository"))
                                        .build_with_ref(&imgui_ui, &mut is_select_repository);
                                    if is_select_repository {
                                        webbrowser::open(REPOSITORY_URL).unwrap();
                                    }
                                    menu.end();
                                }

                                menu_bar.end();
                            }

                            if let Some(menu_bar) = imgui_ui.begin_main_menu_bar() {
                                let mut play = false;
                                MenuItem::new(im_str!("Play")).build_with_ref(&imgui_ui, &mut play);
                                if play {
                                    app.add_message(Message::Play);
                                }
                                menu_bar.end();
                            }

                            let mut w1 = imgui_ui.window_size();
                            imgui::Window::new(im_str!("##ToolBox"))
                                .collapsible(false)
                                .size(
                                    [
                                        300.0,
                                        physical_size
                                            .to_logical::<f64>(window.scale_factor())
                                            .height as f32,
                                    ],
                                    Condition::Always,
                                )
                                .position([0.0, 16.0], Condition::Always)
                                .menu_bar(true)
                                .build(&imgui_ui, || {
                                    if let Some(listbox) =
                                        ListBox::new(im_str!("Entities")).begin(&imgui_ui)
                                    {
                                        for i in 0..10 {
                                            if Selectable::new(im_str!("test"))
                                                .selected(i == 8)
                                                .build(&imgui_ui)
                                            {
                                            }
                                        }
                                        listbox.end();
                                    }

                                    imgui_ui.separator();

                                    imgui_ui.text(im_str!("Hello world!"));
                                    imgui_ui.text(im_str!("This...is...imgui-rs on WGPU!"));
                                    imgui_ui.separator();
                                    let mouse_pos = imgui_ui.io().mouse_pos;
                                    imgui_ui.text(im_str!(
                                        "Mouse Position: ({:.1},{:.1})",
                                        mouse_pos[0],
                                        mouse_pos[1]
                                    ));
                                    imgui_ui.separator();

                                    let mut path = PathBuf::new();
                                    imgui_file_tree(&imgui_ui, &file_tree, &mut path);

                                    w1 = imgui_ui.window_size();
                                });

                            let mut w2 = imgui_ui.window_size();
                            imgui::Window::new(im_str!("##Inspector"))
                                .collapsible(false)
                                .size(
                                    [
                                        300.0,
                                        physical_size
                                            .to_logical::<f64>(window.scale_factor())
                                            .height as f32,
                                    ],
                                    Condition::Always,
                                )
                                .position_pivot([1.0, 0.0])
                                .position(
                                    [
                                        physical_size.to_logical::<f64>(window.scale_factor()).width
                                            as f32,
                                        16.0,
                                    ],
                                    Condition::Always,
                                )
                                .menu_bar(true)
                                .build(&imgui_ui, || {
                                    let mut name = String::from("sample");
                                    imgui_ui.input_text(im_str!("Name"), &mut name);

                                    let mut arr2 = [0f32; 2];
                                    let mut arr3 = [0f32; 3];
                                    imgui_ui
                                        .input_float2(im_str!("Position"), &mut arr2)
                                        .build();
                                    imgui_ui
                                        .input_float3(im_str!("Rotation"), &mut arr3)
                                        .build();
                                    imgui_ui.input_float3(im_str!("Scale"), &mut arr3).build();

                                    w2 = imgui_ui.window_size();
                                });

                            if let Some(window) = imgui::Window::new(im_str!("##Scene"))
                                .size(
                                    [
                                        physical_size.to_logical::<f64>(window.scale_factor()).width
                                            as f32
                                            - w1[0]
                                            - w2[0],
                                        physical_size
                                            .to_logical::<f64>(window.scale_factor())
                                            .height as f32
                                            - 300f32,
                                    ],
                                    Condition::Always,
                                )
                                .position_pivot([0.0, 0.0])
                                .position([w1[0], 16f32], Condition::Always)
                                .title_bar(false)
                                .scroll_bar(false)
                                .scrollable(false)
                                .begin(&imgui_ui)
                            {
                                let window_size = imgui_ui.window_size();
                                if window_size != scene_editor_size {
                                    scene_editor.resize(
                                        &mut renderer,
                                        &WindowSize {
                                            width: window_size[0] as u32,
                                            height: window_size[1] as u32,
                                        },
                                    );
                                    scene_editor_size = window_size;
                                }
                                Image::new(scene_editor.texture_id, scene_editor_size)
                                    .build(&imgui_ui);
                                window.end();
                            }

                            if let Some(window) = imgui::Window::new(im_str!("##Reports"))
                                .title_bar(false)
                                .collapsible(false)
                                .size(
                                    [
                                        physical_size.to_logical::<f64>(window.scale_factor()).width
                                            as f32
                                            - w1[0]
                                            - w2[0],
                                        300.0,
                                    ],
                                    Condition::Always,
                                )
                                .position_pivot([0.0, 1.0])
                                .position(
                                    [
                                        w1[0],
                                        physical_size
                                            .to_logical::<f64>(window.scale_factor())
                                            .height as f32,
                                    ],
                                    Condition::Always,
                                )
                                .begin(&imgui_ui)
                            {
                                TabBar::new(im_str!("##Tab Bar")).build(&imgui_ui, || {
                                    TabItem::new(im_str!("Log")).build(&imgui_ui, || {
                                        if imgui_ui.button(im_str!("Clear")) {
                                            app.add_message(Message::ClearLogger);
                                        }
                                        imgui_ui.same_line();
                                        if imgui_ui.button(im_str!("Copy")) {
                                            imgui_ui.set_clipboard_text(&log_messages);
                                        }
                                        imgui_ui.same_line();
                                        imgui_ui
                                            .input_text(
                                                im_str!("Filter"),
                                                &mut app.ui_state.log_filter,
                                            )
                                            // .resize_buffer(true)
                                            .build();
                                        imgui_ui
                                            .input_text_multiline(
                                                "##Log",
                                                &mut log_messages,
                                                [-1f32, -1f32],
                                            )
                                            .read_only(true)
                                            .build();
                                    });

                                    TabItem::new(im_str!("Build")).build(&imgui_ui, || {
                                        imgui_ui
                                            .input_text_multiline(
                                                im_str!("##Build Log"),
                                                &mut app.ui_state.play_log,
                                                [-1f32, -1f32],
                                            )
                                            .read_only(true)
                                            .build();
                                    });
                                });
                                window.end();
                            }
                        }

                        ui::export_setup::export_setup(&imgui_ui, &mut app.ui_state);
                        ui::settings::settings(&imgui_ui, &mut app.ui_state);
                    }
                };

                let mut encoder: wgpu::CommandEncoder = renderer
                    .state
                    .device
                    .create_command_encoder(&wgpu::CommandEncoderDescriptor { label: None });

                if last_cursor != Some(imgui_ui.mouse_cursor()) {
                    last_cursor = Some(imgui_ui.mouse_cursor());
                    platform.prepare_render(&imgui_ui, &window);
                }

                scene_editor.command_buffer(&renderer);
                let output_view = frame.texture.create_view(&TextureViewDescriptor::default());
                {
                    let mut rpass = encoder.begin_render_pass(&wgpu::RenderPassDescriptor {
                        label: None,
                        color_attachments: &[wgpu::RenderPassColorAttachment {
                            view: &output_view,
                            resolve_target: None,
                            ops: wgpu::Operations {
                                load: wgpu::LoadOp::Clear(wgpu::Color {
                                    r: 0.7,
                                    g: 0.7,
                                    b: 0.6,
                                    a: 1.0,
                                }),
                                store: true,
                            },
                        }],
                        depth_stencil_attachment: None,
                    });

                    renderer
                        .imgui_renderer
                        .render(
                            imgui_ui.render(),
                            &renderer.state.queue,
                            &renderer.state.device,
                            &mut rpass,
                        )
                        .expect("Rendering failed");
                }

                renderer.state.queue.submit(Some(encoder.finish()));
                frame.present();
            }
            _ => (),
        }

        platform.handle_event(imgui_context.io_mut(), &window, &event);
    });
}
