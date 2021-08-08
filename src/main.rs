use wasm_bindgen::prelude::*;

#[wasm_bindgen(start)]
fn main() {
    #[cfg(not(target_os = "android"))]
    env_logger::init();
    #[cfg(target_arch = "wasm32")]
    console_log::init_with_level(console_log::Level::Debug);

    sample::main();
}
