use crate::message::Message;
use crate::ui::UiState;

pub enum AppState {
    ProjectSelection,
    Workspace,
}

pub struct App {
    pub ui_state: UiState,
    pub message_queue: Vec<Message>,
    pub state: AppState,
}

impl App {
    pub fn new() -> Self {
        Self {
            message_queue: Default::default(),
            ui_state: Default::default(),
            state: AppState::Workspace,
        }
    }

    pub fn add_message(&mut self, message: Message) {
        self.message_queue.push(message);
    }

    pub fn frame_init(&mut self) {
        self.message_queue.clear();
    }
}
