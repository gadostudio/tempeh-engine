use std::marker::PhantomData;
use mono_sys::{mono_assembly_close, MonoAssembly};

pub struct Assembly<'a> {
    assembly: *mut MonoAssembly,
    _lifetime_annotation: PhantomData<&'a ()>,
}

impl<'a> Assembly<'a> {
    pub fn new(assembly: *mut MonoAssembly) -> Self {
        Self {
            assembly,
            _lifetime_annotation: Default::default(),
        }
    }
}

impl Drop for Assembly<'_> {
    fn drop(&mut self) {
        unsafe { mono_assembly_close(self.assembly); }
    }
}