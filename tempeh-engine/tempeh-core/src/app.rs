use crate::plugins::Plugin;
use rapier2d::prelude::*;
use tempeh_ecs::storage::IntoComponentSource;
use tempeh_ecs::systems::{Builder as ScheduleBuilder, ParallelRunnable, Resource};
use tempeh_ecs::{Resources, Schedule, World};
use tempeh_engine::{Engine, Physic};

pub struct AppBuilder<W: tempeh_window::TempehWindow + tempeh_window::Runner> {
    name: Option<String>,
    plugins: Vec<Box<dyn Plugin<W>>>,
    schedule_builder: ScheduleBuilder,
    world: World,
    resources: Resources,
    pub window: Option<W>,
}

impl<'a, W: tempeh_window::TempehWindow + tempeh_window::Runner> AppBuilder<W> {
    pub fn new(window: W) -> Self {
        Self {
            name: None,
            plugins: vec![],
            schedule_builder: Schedule::builder(),
            world: World::default(),
            resources: Resources::default(),
            window: Some(window),
        }
    }

    pub fn run(&mut self) {
        let _world = std::mem::replace(&mut self.world, World::default());
        let _resources = std::mem::replace(&mut self.resources, Resources::default());

        self.window.take().unwrap().run(tempeh_engine::Engine {
            world: _world,
            resources: _resources,
            schedule: self.schedule_builder.build(),
            physic: Physic {
                physic_pipeline: PhysicsPipeline::new(),
                query_pipeline: QueryPipeline::new(),
                colliders: ColliderSet::new(),
                rigidbodies: RigidBodySet::new(),
                insland_manager: IslandManager::new(),
            },
        });
        // App {
        //     title: self.name.clone().unwrap_or(String::from("Tempeh Engine")),
        //     engine: Engine {
        //         world: self.world,
        //         resources: self.resources,
        //         schedule: self.schedule_builder.build(),
        //         physic: Physic {
        //             physic_pipeline: PhysicsPipeline::new(),
        //             query_pipeline: QueryPipeline::new(),
        //             colliders: ColliderSet::new(),
        //             rigidbodies: RigidBodySet::new(),
        //             insland_manager: IslandManager::new(),
        //         },
        //     },
        // }
    }

    pub fn add_plugin<T: Plugin<W>>(&mut self, plugin: T) -> &mut Self {
        plugin.inject(self);
        self
    }

    pub fn add_component<T>(&mut self, components: T) -> &mut Self
    where
        Option<T>: IntoComponentSource,
    {
        self.world.push(components);
        self
    }

    pub fn add_system<T: ParallelRunnable + 'static>(&mut self, system: T) -> &mut Self {
        self.schedule_builder.add_system(system);
        self
    }

    pub fn add_resource<T: Resource>(&mut self, resource: T) -> &mut Self {
        self.resources.insert(resource);
        self
    }
}
