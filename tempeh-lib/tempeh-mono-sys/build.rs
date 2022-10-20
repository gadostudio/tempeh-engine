use std::{env, fs};
use std::path::PathBuf;

fn main() -> std::io::Result<()> {
    let manifest_dir = std::path::Path::new(env!("CARGO_MANIFEST_DIR"));
    let output_dir_string = env::var("OUT_DIR").unwrap();
    let output_dir = std::path::Path::new(&output_dir_string);

    let mut source_dir = manifest_dir.to_path_buf();
    source_dir.push("src");

    let mut mono_dir = manifest_dir.to_path_buf();
    mono_dir.push("mono");

    let mut mono_include_dir = mono_dir.clone();
    mono_include_dir.push("include");
    mono_include_dir.push("mono-2.0");

    let mut mono_binary_dir = mono_dir.clone();
    mono_binary_dir.push("win32");
    mono_binary_dir.push("bin");

    println!("FROM {:?} TO {:?}", mono_binary_dir.join("mono-2.0-sgen.dll"), output_dir.join("mono-2.0-sgen.dll"));
    fs::copy(mono_binary_dir.join("mono-2.0-sgen.dll"), output_dir.join("mono-2.0-sgen.dll")).unwrap();

    // Tell cargo to look for shared libraries in the specified directory
    println!("cargo:rustc-link-search=C:\\Users\\andra\\Projects\\mono-rs\\mono-sys\\mono\\win32\\lib");

    // Tell cargo to tell rustc to link the system bzip2
    // shared library.
    println!("cargo:rustc-link-lib=mono-2.0-sgen");

    // Tell cargo to invalidate the built crate whenever the wrapper changes
    println!("cargo:rerun-if-changed=mono/mono.h");

    let bindings = bindgen::Builder::default()
        // The input header we would like to generate
        // bindings for.
        .header("mono/mono.h")
        .clang_arg(&format!("-I{}", mono_include_dir.to_str().unwrap()))
        // Tell cargo to invalidate the built crate whenever any of the
        // included header files changed.
        .parse_callbacks(Box::new(bindgen::CargoCallbacks))
        // Finish the builder and generate the bindings.
        .generate()
        // Unwrap the Result and panic on failure.
        .expect("Unable to generate bindings");

    // Write the bindings to the $OUT_DIR/bindings.rs file.
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(source_dir.join("bindings.rs"))
        .expect("Couldn't write bindings!");

    Ok(())
}