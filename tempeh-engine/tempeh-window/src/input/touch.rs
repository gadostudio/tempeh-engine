use tempeh_math::prelude::*;

#[derive(Debug, Clone)]
pub struct TouchInput {
    pub id: u64,
    pub logical_position: Point2<f64>,
    pub phase: TouchPhase,
}

#[derive(Debug, PartialOrd, PartialEq, Eq, Hash, Clone)]
pub enum TouchPhase {
    Started,
    Moved,
    Ended,
    Cancelled,
}
