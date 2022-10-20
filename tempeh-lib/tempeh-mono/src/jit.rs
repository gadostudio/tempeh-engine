use std::ffi::OsStr;
use std::os::raw::c_char;
use std::path::Path;
use mono_sys::{mono_domain_assembly_open, mono_jit_cleanup, mono_jit_init, MonoDomain};
use crate::assembly::Assembly;
use crate::util::path_to_c_str;
use std::marker::PhantomData;

pub struct JIT<'a> {
    domain: *mut MonoDomain,
    _lifetime_annotation: PhantomData<&'a ()>,
}

impl<'a> JIT<'a> {
    pub fn new(name: &str) -> Self {
        let maybe_internal = unsafe { mono_jit_init(name.as_ptr() as *const c_char) };
        if maybe_internal.is_null() {
            panic!("Domain creation failed");
        }
        Self {
            domain: maybe_internal,
            _lifetime_annotation: Default::default(),
        }
    }

    pub fn assembly_open<P: AsRef<Path>>(&mut self, file: P) -> Assembly<'a> {
        let maybe_assemby = unsafe {
            mono_domain_assembly_open(self.domain, path_to_c_str(file.as_ref()).as_ptr())
        };
        if maybe_assemby.is_null() {
            panic!("Domain open failed");
        }
        Assembly::new(maybe_assemby)
    }
}

impl Drop for JIT<'_> {
    fn drop(&mut self) {
        unsafe { mono_jit_cleanup(self.domain); }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    // #[test]
    // fn create_jit() {
    //     JIT::new("adder");
    // }

    #[test]
    fn load_assembly() {
        let manifest_dir = std::path::Path::new(env!("CARGO_MANIFEST_DIR"));

        let mut source_dir = manifest_dir.to_path_buf();
        source_dir.pop();
        source_dir.push("test");
        source_dir.push("fixtures");
        source_dir.push("adder");

        println!("TEST {:?}", source_dir.join("adder.dll"));

        let mut adder_jit = JIT::new("adder");
        adder_jit.assembly_open(source_dir.join("adder.dll"));
    }
}
