use libloading::{Library, Symbol};

type SceneEntry = unsafe fn() -> ();

pub fn run_game() {
    unsafe {
        let lib = Library::new(
            "C:\\Users\\andra\\Projects\\tempeh-engine\\target\\debug\\hello_world.dll",
        )
        .unwrap();
        let func: Symbol<SceneEntry> = lib.get(b"entry_scene").unwrap();
        func();
    }
}
