use std::time::Duration;
use tempeh_ecs::prelude::*;
use tempeh_window::input::keyboard::VirtualKeyCode;
use tempeh_window::input::InputManager;
use tempeh_window::Runner;
use tempeh_window_winit::WinitWindow;

#[derive(Clone, Copy, Debug, PartialEq)]
pub struct Position {
    pub x: f32,
    pub y: f32,
}

#[derive(Clone, Copy, Debug, PartialEq)]
pub struct Velocity {
    pub dx: f32,
    pub dy: f32,
}

#[system(for_each)]
fn update_positions(
    pos: &mut Position,
    vel: &Velocity,
    #[resource] delta_time: &Duration,
    #[resource] input: &InputManager,
) {
    if input.is_key_pressed(VirtualKeyCode::A) {
        log::warn!("Left");
    }
    // for keyboard_press in input.keyboard_presses() {
    //     log::warn!("{:?}", keyboard_press);
    // }
    // pos.x += vel.dx * delta_time.as_secs_f32();
    // pos.y += vel.dy * delta_time.as_secs_f32();
}

#[cfg_attr(
    target_os = "android",
    ndk_glue::main(backtrace = "on", logger(level = "debug", tag = "tempeh-engine"))
)]
pub fn main() {
    #[cfg(all(not(target_os = "android"), not(target_arch = "wasm32")))]
    env_logger::init();
    #[cfg(target_arch = "wasm32")]
    {
        console_log::init_with_level(log::Level::Debug);
    }
    let mut game =
        tempeh_core::game::Game::new(String::from("Platformer"), vec![update_positions_system()]);
    game.engine
        .world
        .push((Position { x: 0.0, y: 0.0 }, Velocity { dx: 0.0, dy: 0.0 }));
    WinitWindow::new().run(game.engine);
}
