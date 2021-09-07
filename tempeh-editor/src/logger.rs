use log::{Level, LevelFilter, Log, Metadata, Record, SetLoggerError};
use once_cell::sync::Lazy;
use std::sync::{Arc, Mutex};

pub trait Filter {
    fn is_pass(&self, string: &str) -> bool;
}

struct FilterContains<'a> {
    string: &'a str,
}

impl Filter for FilterContains<'_> {
    fn is_pass(&self, string: &str) -> bool {
        string.contains(self.string)
    }
}

pub static LOG_MESSAGES: Lazy<Mutex<Vec<LogMessage>>> = Lazy::new(|| {
    let v = Vec::<LogMessage>::new_clamped_capacity();
    Mutex::new(v)
});

const CLAMPED_VEC_CAPACITY: usize = 400;

trait ClampedVec<T> {
    fn new_clamped_capacity() -> Self;
    fn push_clamped(&mut self, value: T);
}

impl<T> ClampedVec<T> for Vec<T> {
    fn new_clamped_capacity() -> Self {
        Vec::<T>::with_capacity(CLAMPED_VEC_CAPACITY)
    }

    fn push_clamped(&mut self, value: T) {
        if self.len() >= CLAMPED_VEC_CAPACITY {
            self.remove(0);
        }
        self.push(value);
    }
}

pub struct LogMessage {
    pub target: String,
    pub message: String,
}

pub struct Logger {
    pub filter: Option<Box<dyn Filter + Send + Sync>>,
}

impl Log for Logger {
    fn enabled(&self, metadata: &Metadata) -> bool {
        true
    }

    fn log(&self, record: &Record) {
        if let Some(filter) = &self.filter {
            if !filter.is_pass(record.target()) {
                return;
            }
        }

        LOG_MESSAGES.lock().unwrap().push_clamped(LogMessage {
            target: record.target().to_string(),
            message: record.args().to_string(),
        });
    }

    fn flush(&self) {}
}

static LOGGER: Logger = Logger { filter: None };

pub fn init() -> Result<(), SetLoggerError> {
    log::set_logger(&LOGGER).map(|()| log::set_max_level(LevelFilter::Warn))
}
