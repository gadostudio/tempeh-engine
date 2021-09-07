use clipboard::{ClipboardContext, ClipboardProvider};
use imgui::{ClipboardBackend, ImStr, ImString};
use std::error::Error;

pub struct Clipboard(ClipboardContext);

impl Clipboard {
    pub fn new() -> Result<Self, Box<dyn Error>> {
        ClipboardContext::new().map(|context| Clipboard(context))
    }

    pub fn get(&mut self) -> Result<String, Box<dyn Error>> {
        self.0.get_contents()
    }

    pub fn set(&mut self, value: String) {
        self.0.set_contents(value);
    }
}

impl ClipboardBackend for Clipboard {
    fn get(&mut self) -> Option<ImString> {
        self.0.get_contents().ok().map(|text| text.into())
    }

    fn set(&mut self, value: &ImStr) {
        self.0.set_contents(value.to_string());
    }
}
