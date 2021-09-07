use std::path::{Path, PathBuf};

pub struct Project {
    path: Option<PathBuf>,
}

impl Project {
    pub fn new() -> Self {
        Self { path: None }
    }

    pub fn open<P: Into<PathBuf>>(&mut self, path: P) {
        self.path = Some(PathBuf::from(path.into()));
    }
}
