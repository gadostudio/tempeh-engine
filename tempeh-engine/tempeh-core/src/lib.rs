pub mod app;
pub mod plugins;

pub use app::AppBuilder;

pub mod prelude {
    pub use crate::AppBuilder;
}

pub use tempeh_core_codegen::main;
