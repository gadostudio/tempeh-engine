use core::marker::PhantomData;
use tempeh_miniaudio_sys::{ma_sound, ma_sound_start, ma_sound_set_looping, ma_sound_set_position, ma_sound_uninit};

pub struct Sound<'a> {
    pub(crate) sound: ma_sound,
    pub(crate) _engine_lifetime: PhantomData<&'a ()>,
}

impl<'a> Sound<'a> {
    pub(crate) fn from_ma_sound(sound: ma_sound) -> Self {
        Self {
            sound,
            _engine_lifetime: PhantomData::default(),
        }
    }

    pub fn start(&mut self) {
        unsafe { ma_sound_start(&mut self.sound as *mut ma_sound) };
    }

    pub fn set_looping(&mut self, is_looping: bool) {
        unsafe { ma_sound_set_looping(&mut self.sound as *mut ma_sound, is_looping) };
    }

    pub fn set_position(&mut self) {
        unsafe {
            ma_sound_set_position(&mut self.sound as *mut ma_sound)
        };
    }
}

impl<'a> Drop for Sound<'a> {
    fn drop(&mut self) {
        unsafe { ma_sound_uninit(&mut self.sound as *mut ma_sound) };
    }
}

