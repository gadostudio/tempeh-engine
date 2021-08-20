pub use log::*;

pub fn init() {
    #[cfg(all(not(target_os = "android"), not(target_arch = "wasm32")))]
    env_logger::init();
    #[cfg(target_arch = "wasm32")]
    console_log::init();
    #[cfg(target_os = "android")]
    android_logger::init_once(
        android_logger::Config::default()
            .with_tag("tempeh-engine")
            .with_min_level(log::Level::Debug),
    );
}
