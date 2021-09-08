use proc_macro::TokenStream;
use std::str::FromStr;

#[proc_macro_attribute]
pub fn main(_attr: TokenStream, tokens: TokenStream) -> TokenStream {
    let mut new_tokens = TokenStream::new();
    new_tokens.extend(
        TokenStream::from_str(
            "#[cfg(target_os = \"android\")]\n
#[no_mangle]\n
unsafe extern \"C\" fn ANativeActivity_onCreate(\n
    activity: *mut std::os::raw::c_void,\n
    saved_state: *mut std::os::raw::c_void,\n
    saved_state_size: usize,\n
) {\n
    std::env::set_var(\"RUST_BACKTRACE\", \"1\");\n
    ndk_glue::init(activity as _, saved_state as _, saved_state_size as _, main);\n
}",
        )
        .unwrap(),
    );
    new_tokens.extend(tokens);
    new_tokens
}
