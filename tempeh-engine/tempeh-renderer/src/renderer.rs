use crate::mini_block::BlockingFuture;
use crate::state::State;
use crate::VERTICES;
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
            state: State::new(window, screen_size).block(),
            clear_color,
            command_buffer_queue: Some(vec![]),
        }
    }

    pub fn resize(&mut self, size: ScreenSize) {}

    pub fn set_clear_color(&mut self, clear_color: wgpu::Color) {
        self.clear_color = clear_color;
    }
}
