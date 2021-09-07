use imgui::ImString;

pub mod export_setup;
pub mod filetree;
pub mod settings;

pub struct UiState {
    pub(crate) log_filter: ImString,
    pub(crate) play_log: ImString,
    pub(crate) display_export_setup_window: bool,
    pub(crate) display_settings_window: bool,
}

impl UiState {
    pub fn new() -> Self {
        Self {
            log_filter: ImString::new(""),
            play_log: ImString::new(""),
            display_export_setup_window: false,
            display_settings_window: false,
        }
    }
}

impl Default for UiState {
    fn default() -> Self {
        Self::new()
    }
}
