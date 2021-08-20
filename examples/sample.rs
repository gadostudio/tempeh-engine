use std::time::Duration;
use tempeh_core::prelude::*;
use tempeh_core_component::prelude::*;
use tempeh_ecs::prelude::*;
use tempeh_log::warn;
use tempeh_renderer::prelude::*;
use tempeh_window::input::keyboard::VirtualKeyCode;
use tempeh_window::input::InputManager;
use tempeh_window::Runner;
use tempeh_window_winit::WinitWindow;

#[derive(Clone, Copy, Debug, PartialEq)]
pub struct Velocity {
    pub dx: f32,
    pub dy: f32,
}

#[system(for_each)]
fn update_positions(
    vel: &Velocity,
    #[resource] delta_time: &Duration,
    #[resource] input: &InputManager,
) {
    if input.is_key_pressed(VirtualKeyCode::A) {
        warn!("Left");
    }
    // for keyboard_press in input.keyboard_presses() {
    //     log::warn!("{:?}", keyboard_press);
    // }
    // pos.x += vel.dx * delta_time.as_secs_f32();
    // pos.y += vel.dy * delta_time.as_secs_f32();
}

#[cfg_attr(target_os = "android", ndk_glue::main(backtrace = "on"))]
pub fn main() {
    tempeh_log::init();

    let mut app = App::builder()
        .add_plugin(RendererPlugin::default())
        .add_system(update_positions_system())
        .build();
    app.engine
        .world
        .push((Transform::default(), Velocity { dx: 0.0, dy: 0.0 }));
    WinitWindow::new().run(app.engine);
}
