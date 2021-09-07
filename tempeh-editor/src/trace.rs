use crate::message::Message;
use std::collections::VecDeque;

const MAX_TRACE: usize = usize::MAX;

pub struct UndoTrace {
    actions: VecDeque<Message>,
}

impl UndoTrace {
    fn push(&mut self, message: Message) {
        if self.actions.len() == MAX_TRACE {
            self.actions.pop_front();
        }
        self.actions.push_back(message);
    }
}
