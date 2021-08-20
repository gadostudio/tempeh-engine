use crate::plugins::Plugin;
use rapier2d::prelude::*;
use tempeh_ecs::systems::{Builder as ScheduleBuilder, ParallelRunnable};
use tempeh_ecs::{Resources, Schedule, World};

pub struct Physic {
    pub physic_pipeline: PhysicsPipeline,
    pub query_pipeline: QueryPipeline,
    pub rigidbodies: RigidBodySet,
    pub colliders: ColliderSet,
    pub insland_manager: IslandManager,
}

pub struct Engine {
    pub world: World,
    pub resources: Resources,
    pub schedule: Schedule,
    pub physic: Physic,
}

pub struct App {
    pub title: String,
    pub engine: Engine,
}

impl App {
    pub fn builder() -> AppBuilder {
        AppBuilder::new()
    }
}

pub struct AppBuilder {
    name: Option<String>,
    plugins: Vec<Box<dyn Plugin>>,
    schedule_builder: ScheduleBuilder,
}

impl<'a> AppBuilder {
    pub fn new() -> Self {
        Self {
            name: None,
            plugins: vec![],
            schedule_builder: Schedule::builder(),
        }
    }

    pub fn build(&mut self) -> App {
        App {
            title: self.name.clone().unwrap_or(String::from("Tempeh Engine")),
            engine: Engine {
                world: World::default(),
                resources: Resources::default(),
                schedule: self.schedule_builder.build(),
                physic: Physic {
                    physic_pipeline: PhysicsPipeline::new(),
                    query_pipeline: QueryPipeline::new(),
                    colliders: ColliderSet::new(),
                    rigidbodies: RigidBodySet::new(),
                    insland_manager: IslandManager::new(),
                },
            },
        }
    }

    pub fn add_plugin<T: Plugin>(&mut self, plugin: T) -> &mut Self {
        plugin.inject(self);
        self
    }

    pub fn add_system<T: ParallelRunnable + 'static>(&mut self, system: T) -> &mut Self {
        self.schedule_builder.add_system(system);
        self
    }
}
