use std::ops::Add;
use std::time::Duration;
use tempeh_core::prelude::*;
use tempeh_core_component::prelude::*;
use tempeh_ecs::prelude::*;
use tempeh_log::warn;
use tempeh_renderer::prelude::*;
use tempeh_window::input::keyboard::VirtualKeyCode;
use tempeh_window::input::mouse::MouseButton;
use tempeh_window::input::InputManager;
use tempeh_window::Runner;
use tempeh_window_winit::WinitWindow;

#[system(for_each)]
fn update_positions(
    transform: &mut Transform,
    #[resource] input_manager: &InputManager,
    #[resource] delta_time: &Duration,
) {
    tempeh_log::warn!("ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ");
    if input_manager.is_key_pressed(&VirtualKeyCode::A) {
        transform.position = transform.position.add(tempeh_math::Vector2::<f32>::new(
            -10000.0 * delta_time.as_secs_f32(),
            0.0,
        ));
    }
    if input_manager.is_key_pressed(&VirtualKeyCode::D) {
        transform.position = transform.position.add(tempeh_math::Vector2::<f32>::new(
            10000.0 * delta_time.as_secs_f32(),
            0.0,
        ));
    }
    if input_manager.is_key_pressed(&VirtualKeyCode::W)
        || input_manager.is_mouse_pressed(&MouseButton::Left)
        || input_manager.touch_presses.len() > 0
    {
        transform.position = transform.position.add(tempeh_math::Vector2::<f32>::new(
            0.0,
            10000.0 * delta_time.as_secs_f32(),
        ));
    }
    if input_manager.is_key_pressed(&VirtualKeyCode::S)
        || input_manager.is_mouse_pressed(&MouseButton::Right)
    {
        transform.position = transform.position.add(tempeh_math::Vector2::<f32>::new(
            0.0,
            -10000.0 * delta_time.as_secs_f32(),
        ));
    }
}

#[cfg_attr(target_os = "android", ndk_glue::main(backtrace = "on"))]
pub fn main() {
    tempeh_log::init();

    AppBuilder::new(WinitWindow::new())
        .add_plugin(RendererPlugin::default())
        .add_system(update_positions_system())
        .run();
}
