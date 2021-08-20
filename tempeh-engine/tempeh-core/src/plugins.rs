use crate::app::AppBuilder;

pub trait Plugin {
    fn inject(&self, app: &mut AppBuilder);
}
