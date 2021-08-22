use crate::app::AppBuilder;

pub trait Plugin<W: tempeh_window::TempehWindow + tempeh_window::Runner> {
    fn inject(&self, app: &mut AppBuilder<W>);
}
