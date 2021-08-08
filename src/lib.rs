use legion::system;
use std::time::Duration;
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
fn update_positions(pos: &mut Position, vel: &Velocity, #[resource] delta_time: &Duration) {
    pos.x += vel.dx * delta_time.as_secs_f32();
    pos.y += vel.dy * delta_time.as_secs_f32();
    println!("{:?}", pos);
}

#[cfg_attr(
    target_os = "android",
    ndk_glue::main(backtrace = "on", logger(level = "debug", tag = "tempeh-engine"))
)]
pub fn main() {
    let mut game = tempeh_core::game::Game::new(String::from("Platformer"));
    game.engine
        .world
        .push((Position { x: 0.0, y: 0.0 }, Velocity { dx: 0.0, dy: 0.0 }));
    let mut w = WinitWindow::new();
    w.run(game.engine);
}
