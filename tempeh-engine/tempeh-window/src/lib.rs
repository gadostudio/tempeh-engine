pub mod input;

use tempeh_core::app::Engine;

pub trait TempehWindow {
    fn focus(&mut self);
    fn set_title(&mut self, title: &str);
}

pub trait Runner {
    fn run(self, engine: Engine);
}
