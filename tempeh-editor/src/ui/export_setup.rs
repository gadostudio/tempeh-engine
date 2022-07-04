use crate::ui::UiState;
use imgui::{im_str, ListBox, Ui, Window};

pub fn export_setup(imgui_ui: &Ui, ui_state: &mut UiState) {
    if ui_state.display_export_setup_window {
        Window::new(im_str!("Export"))
            .opened(&mut ui_state.display_export_setup_window)
            .collapsible(false)
            .build(&imgui_ui, || {
                imgui_ui.columns(2, im_str!("##Export"), false);
                imgui_ui.text(im_str!("Presets"));
                imgui_ui.push_item_width(-1f32);
                if let Some(listbox) = ListBox::new(im_str!("##Presets")).begin(&imgui_ui) {
                    listbox.end();
                }
                imgui_ui.next_column();
                imgui_ui.text(im_str!("blablabla"));

                imgui_ui.columns(1, im_str!("##Export Buttons"), false);
                imgui_ui.button(im_str!("Export"));
            });
    }
}
