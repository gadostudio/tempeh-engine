use async_std::channel::SendError;
use async_std::channel::Sender;
use async_std::io::BufReader;
use async_std::prelude::*;
use async_std::process::{Child, Command, Stdio};
use std::io;
use std::path::PathBuf;

pub enum ToolchainError {
    IOError(io::Error),
    ChannelSendError(SendError<String>),
}

impl From<io::Error> for ToolchainError {
    fn from(err: io::Error) -> Self {
        ToolchainError::IOError(err)
    }
}

impl From<SendError<String>> for ToolchainError {
    fn from(err: SendError<String>) -> Self {
        ToolchainError::ChannelSendError(err)
    }
}

pub struct Toolchain {
    pub path: PathBuf,
}

impl Toolchain {
    pub async fn from() -> Self {
        let toolchain_dir = PathBuf::from("C:/Users/andra/.cargo/bin/");
        let cargo_path = toolchain_dir.join("cargo.exe");

        let cargo_run = Command::new(&cargo_path)
            .arg("-v")
            .stderr(Stdio::null())
            .stdout(Stdio::null())
            .status();

        if let Ok(exit_status) = cargo_run.await {
            if !cargo_path.is_file() || !exit_status.success() {}
        }

        Self {
            path: toolchain_dir,
        }
    }

    pub fn compile(&self, pwd: &str) -> Result<Child, ToolchainError> {
        let cargo_path = self.path.join("cargo.exe");
        Ok(Command::new(&cargo_path)
            .current_dir(pwd)
            .args(["build", "--lib"])
            .stdout(Stdio::piped())
            .stderr(Stdio::piped())
            .spawn()?)
    }

    pub async fn compile_and_record(
        &self,
        recorder: &Sender<String>,
        pwd: &str,
    ) -> Result<(), ToolchainError> {
        let mut child = self.compile(pwd)?;
        let mut stderr_lines = BufReader::new(child.stderr.take().unwrap()).lines();
        loop {
            if let Some(stderr_line) = stderr_lines.next().await {
                recorder.send(stderr_line?).await?;
            } else {
                break;
            }
        }
        Ok(())
    }
}
