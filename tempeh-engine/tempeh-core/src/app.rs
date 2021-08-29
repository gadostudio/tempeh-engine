use crate::plugins::Plugin;
use rapier2d::prelude::*;
use tempeh_ecs::storage::IntoComponentSource;
use tempeh_ecs::systems::{Executor, ParallelRunnable, Resource, Step};
use tempeh_ecs::{Resources, Schedule, World};
use tempeh_engine::{Engine, Physic};

struct Systems {
    startup_system: Vec<Box<dyn ParallelRunnable + 'static>>,
    preupdate_system: Vec<Box<dyn ParallelRunnable + 'static>>,
    update_system: Vec<Box<dyn ParallelRunnable + 'static>>,
    postupdate_system: Vec<Box<dyn ParallelRunnable + 'static>>,
}

pub struct AppBuilder<W: tempeh_window::TempehWindow + tempeh_window::Runner> {
    name: Option<String>,
    plugins: Vec<Box<dyn Plugin<W>>>,
    world: World,
    resources: Resources,
    pub window: Option<W>,
    systems: Systems,
}

impl<'a, W: tempeh_window::TempehWindow + tempeh_window::Runner> AppBuilder<W> {
    pub fn new(window: W) -> Self {
        Self {
            name: None,
            plugins: vec![],
            world: World::default(),
            resources: Resources::default(),
            window: Some(window),
            systems: Systems {
                startup_system: vec![],
                preupdate_system: vec![],
                update_system: vec![],
                postupdate_system: vec![],
            },
        }
    }

    pub fn run(&mut self) {
        let mut _world = std::mem::replace(&mut self.world, World::default());
        let mut _resources = std::mem::replace(&mut self.resources, Resources::default());

        let mut startup_schedule_steps = Vec::new();
        if self.systems.startup_system.len() > 0 {
            let mut systems_consumer = Vec::new();
            std::mem::swap(&mut self.systems.startup_system, &mut systems_consumer);
            startup_schedule_steps.push(Step::Systems(Executor::new(systems_consumer)));
            startup_schedule_steps.push(Step::FlushCmdBuffers);
            Schedule::from(startup_schedule_steps).execute(&mut _world, &mut _resources);
        }

        let mut schedule_steps = Vec::new();
        if self.systems.preupdate_system.len() > 0 {
            let mut systems_consumer = Vec::new();
            std::mem::swap(&mut self.systems.preupdate_system, &mut systems_consumer);
            schedule_steps.push(Step::Systems(Executor::new(systems_consumer)));
            schedule_steps.push(Step::FlushCmdBuffers);
        }
        if self.systems.update_system.len() > 0 {
            let mut systems_consumer = Vec::new();
            std::mem::swap(&mut self.systems.update_system, &mut systems_consumer);
            schedule_steps.push(Step::Systems(Executor::new(systems_consumer)));
            schedule_steps.push(Step::FlushCmdBuffers);
        }
        if self.systems.postupdate_system.len() > 0 {
            let mut systems_consumer = Vec::new();
            std::mem::swap(&mut self.systems.postupdate_system, &mut systems_consumer);
            schedule_steps.push(Step::Systems(Executor::new(systems_consumer)));
            schedule_steps.push(Step::FlushCmdBuffers);
        }

        self.window.take().unwrap().run(tempeh_engine::Engine {
            world: _world,
            resources: _resources,
            schedule: Schedule::from(schedule_steps),
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

    pub fn add_startup_system<T: ParallelRunnable + 'static>(&mut self, system: T) -> &mut Self {
        self.systems.startup_system.push(Box::new(system));
        self
    }

    pub fn add_preupdate_system<T: ParallelRunnable + 'static>(&mut self, system: T) -> &mut Self {
        self.systems.preupdate_system.push(Box::new(system));
        self
    }

    pub fn add_system<T: ParallelRunnable + 'static>(&mut self, system: T) -> &mut Self {
        self.systems.update_system.push(Box::new(system));
        self
    }

    pub fn add_postupdate_system<T: ParallelRunnable + 'static>(&mut self, system: T) -> &mut Self {
        self.systems.postupdate_system.push(Box::new(system));
        self
    }

    pub fn add_resource<T: Resource>(&mut self, resource: T) -> &mut Self {
        self.resources.insert(resource);
        self
    }
}
