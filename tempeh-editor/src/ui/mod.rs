use imgui::ImString;

pub mod export_setup;
pub mod filetree;
pub mod settings;

pub struct UiState {
    pub(crate) log_filter: String,
    pub(crate) play_log: String,
    pub(crate) display_export_setup_window: bool,
    pub(crate) display_settings_window: bool,
}

impl UiState {
    pub fn new() -> Self {
        Self {
            log_filter: String::new(),
            play_log: String::new(),
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
