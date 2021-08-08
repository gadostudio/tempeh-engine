fn main() {
    #[cfg(all(not(target_os = "android"), not(target_arch = "wasm32")))]
    env_logger::init();
    #[cfg(target_arch = "wasm32")]
    {
        console_log::init_with_level(log::Level::Debug);
        log::debug!("test");
    }

    sample::main();
}
