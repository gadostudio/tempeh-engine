use std::ffi::OsStr;
use std::path::Path;
use std::process::Command;

pub fn open_file<S: AsRef<OsStr>>(file: S) {
    if cfg!(target_os = "windows") {
        Command::new("explorer")
            .arg(file.as_ref())
            .output()
            .unwrap();
    } else {
        Command::new("xdg-open")
            .arg(file.as_ref())
            .output()
            .unwrap();
    }
}
