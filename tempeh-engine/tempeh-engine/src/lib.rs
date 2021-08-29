use rapier2d::prelude::*;
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
