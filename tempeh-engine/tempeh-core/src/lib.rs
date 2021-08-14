pub mod ecs;
pub mod game;

use log::info;
use std::ffi::{CStr, CString};
use std::io::{BufReader, Cursor, Read};
use std::sync::{Arc, Mutex};
use tempeh_renderer;
use tempeh_renderer::state::State;

pub fn main() {
    // #[cfg(target_os = "android")]
    // {
    //     let native_activity = ndk_glue::native_activity();
    //     let asset_manager = native_activity.asset_manager();
    //     let asset = asset_manager
    //         .open(&CString::new("imperial.ogg").unwrap())
    //         .unwrap();
    //     let mut data = Vec::with_capacity(asset.get_length());
    //     BufReader::new(asset).read_to_end(&mut data).unwrap();
    //     let (_stream, stream_handle) = rodio::OutputStream::try_default().unwrap();
    //     stream_handle.play_once(Cursor::new(data));
    // }
}
