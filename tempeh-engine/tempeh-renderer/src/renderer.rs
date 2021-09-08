use crate::state::State;
use async_std::task;
use tempeh_window::ScreenSize;

pub struct Renderer {
    pub state: State,
    pub clear_color: wgpu::Color,
    pub command_buffer_queue: Option<Vec<wgpu::CommandBuffer>>,
}

impl Renderer {
    pub fn new(
        window: &impl tempeh_window::HasRawWindowHandle,
        screen_size: ScreenSize,
        clear_color: wgpu::Color,
    ) -> Self {
        Self {
            state: task::block_on(State::new(window, screen_size)),
            clear_color,
            command_buffer_queue: Some(vec![]),
        }
    }
}
