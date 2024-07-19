use std::ffi::CStr;

pub mod console;

fn main() {
    let mut cle = console::Console::new().unwrap();
    println!("Type any key.");

    cle.run_loop(&mut (), |_, meta| {
        let cstr = unsafe { CStr::from_ptr(meta.input as *const _) };

        println!(
            "{:?}",
            String::from_utf8_lossy(cstr.to_bytes()).to_string() //.to_str().unwrap()
        );
        console::KeyResponse::Stop
    })
    .unwrap();
}
