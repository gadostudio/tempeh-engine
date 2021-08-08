use legion::{Resources, Schedule, World};
use std::time::Duration;

pub struct Engine {
    pub world: World,
    pub resources: Resources,
    pub schedule: Schedule,
}

pub struct Game {
    pub title: String,
    pub engine: Engine,
}

impl Game {
    pub fn new(title: String) -> Self {
        Self {
            title,
            engine: Engine {
                world: World::default(),
                resources: Resources::default(),
                schedule: Schedule::builder().build(),
            },
        }
    }
}
