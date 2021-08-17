use tempeh_math::prelude::*;

#[derive(Debug, Clone)]
pub struct TouchInput {
    id: u64,
    logical_position: Point2<f64>,
    phase: TouchPhase,
}

#[derive(Debug, Clone)]
pub enum TouchPhase {
    Started,
    Moved,
    Ended,
    Cancelled,
}
