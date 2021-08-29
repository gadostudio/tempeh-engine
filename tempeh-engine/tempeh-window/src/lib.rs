pub mod input;

use tempeh_engine::Engine;

pub use raw_window_handle::HasRawWindowHandle;

pub struct ScreenSize {
    pub width: u32,
    pub height: u32,
}

pub trait TempehWindow {
    type Window: HasRawWindowHandle;

    fn focus(&mut self);
    fn set_title(&mut self, title: &str);
    fn get_window_size(&self) -> ScreenSize;
    fn get_raw_window_handle(&self) -> &Self::Window;
}

pub trait Runner {
    fn run(self, engine: Engine);
}
