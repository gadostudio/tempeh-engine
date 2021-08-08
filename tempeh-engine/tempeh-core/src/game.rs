use legion::{Resources, Schedule, World};
use rapier2d::prelude::*;

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
    pub fn new(title: String) -> Self {
        Self {
            title,
            engine: Engine {
                world: World::default(),
                resources: Resources::default(),
                schedule: Schedule::builder().build(),
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
