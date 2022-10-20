use std::ffi::CString;
use std::path::Path;

pub fn path_to_c_str(path: &Path) -> CString {
    let mut buf = Vec::new();

    #[cfg(unix)] {
        use std::os::unix::ffi::OsStrExt;
        buf.extend(path.as_os_str().as_bytes());
        buf.push(0);
    }

    #[cfg(windows)] {
        use std::os::windows::ffi::OsStrExt;
        buf.extend(path.as_os_str()
            .encode_wide()
            .chain(Some(0))
            .map(|b| {
                let b = b.to_ne_bytes();
                b.get(0).map(|s| *s).into_iter().chain(b.get(1).map(|s| *s))
            })
            .flatten());
    }
    unsafe { CString::from_vec_unchecked(buf) }
}