use crate::ui::UiState;
use imgui::{im_str, ComboBox, ListBox, Ui, Window};

pub fn settings(imgui_ui: &Ui, ui_state: &mut UiState) {
    if ui_state.display_settings_window {
        Window::new(im_str!("Export"))
            .opened(&mut ui_state.display_settings_window)
            .collapsible(false)
            .build(&imgui_ui, || {
                // ComboBox::new("Theme").build_simple_string(&imgui_ui, &mut 0, &["Light", "Dark"]);
            });
    }
}
