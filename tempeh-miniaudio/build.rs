extern crate bindgen;

use std::env;
use std::path::PathBuf;

fn main() {
    // // Tell cargo to look for shared libraries in the specified directory
    // println!("cargo:rustc-link-search=/path/to/lib");

    // // Tell cargo to tell rustc to link the system bzip2
    // // shared library.
    // println!("cargo:rustc-link-lib=bz2");

    println!("cargo:rerun-if-changed=miniaudio/miniaudio.h");

    let bindings = bindgen::Builder::default()
        .header("miniaudio/miniaudio.h")
        .parse_callbacks(Box::new(bindgen::CargoCallbacks))
        .generate()
        .unwrap();

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");
}
