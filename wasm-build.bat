# https://github.com/gfx-rs/wgpu/wiki/Running-on-the-Web-with-WebGPU-and-WebGL
set RUSTFLAGS=--cfg=web_sys_unstable_apis && cargo build --no-default-features --target wasm32-unknown-unknown --example sample
wasm-bindgen --out-dir target/generated --web target/wasm32-unknown-unknown/debug/examples/sample.wasm
copy assets\web\* target\generated
miniserve target/generated
