use core::ffi::CStr;

use crate::sound::Sound;
use tempeh_miniaudio_sys::{ma_engine, ma_engine_init, ma_result_MA_SUCCESS, ma_engine_uninit, ma_engine_play_sound, ma_sound, ma_sound_init_from_file};
use crate::error::Error;

pub struct Engine {
    engine: ma_engine,
}

impl<'a> Engine {
    pub fn new() -> Result<Self, Error> {
        let mut engine = unsafe { core::mem::MaybeUninit::uninit().assume_init() };
        let result = unsafe { ma_engine_init(core::ptr::null(), &mut engine as *mut ma_engine) };
        Error::from(result).map(|_| Self { engine })
    }

    pub fn play_sound(&mut self, file_path: CStr) -> Result<Sound<'a>, Error> {
        let mut sound = unsafe { core::mem::MaybeUninit::uninit().assume_init() };
        let result = unsafe {
            ma_engine_play_sound(
                &mut self.engine as *mut ma_engine,
                file_path,
                &mut sound as *mut ma_sound,
            )
        };
        Error::from(result).map(|_| Sound::from_ma_sound(sound))
    }

    pub fn create_sound_from_file(&mut self, file_path: CStr) -> Sound<'a> {
        let mut sound = unsafe { core::mem::MaybeUninit::uninit().assume_init() };
        let result = unsafe {
            ma_sound_init_from_file(
                &mut self.engine as *mut ma_engine,
                file_path,
                0,
                Optionalma_sound_group,
                Optionalma_fence,
                &mut sound as *mut ma_sound,
            );
        };
        Error::from(result).map(|_| Sound::from_ma_sound(sound))
    }
}

impl Drop for Engine {
    fn drop(&mut self) {
        unsafe {
            ma_engine_uninit(&mut self.engine as *mut ma_engine);
        }
    }
}
