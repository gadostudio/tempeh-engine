pub mod miniaudio_raw {
    include!(concat!(env!("OUT_DIR"), "/bindings.rs"));
}

pub struct Engine {
    engine: ma_engine
}

impl Engine {
    pub fn new() -> Self {
        let result = ma_encoder_init(onWrite, onSeek, pUserData, pConfig, pEncoder);
        Self {
            engine
        }
    }
}
