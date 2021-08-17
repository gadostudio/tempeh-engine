use rapier2d::prelude::*;
use tempeh_ecs::systems::ParallelRunnable;
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

pub struct Game {
    pub title: String,
    pub engine: Engine,
}

impl Game {
    pub fn new<T: ParallelRunnable + 'static>(title: String, systems: Vec<T>) -> Self {
        let mut schedule_builder = Schedule::builder();
        for system in systems.into_iter() {
            schedule_builder.add_system(system);
        }
        let schedule = schedule_builder.build();
        Self {
            title,
            engine: Engine {
                world: World::default(),
                resources: Resources::default(),
                schedule,
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
}
