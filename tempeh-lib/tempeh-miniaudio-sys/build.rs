fn main() {
    println!("cargo:rerun-if-changed=miniaudio/miniaudio.c");

    cc::Build::new()
        .file("miniaudio/miniaudio.c")
        .define("MINIAUDIO_IMPLEMENTATION", "")
        .compile("miniaudio");
}
