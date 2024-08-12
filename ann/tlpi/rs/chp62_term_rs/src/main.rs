use std::ffi::CStr;
use std::mem;
use std::os::raw::c_char;

#[path = "console.rs"]
pub mod console_lib;

fn main() {
    let mut console = console_lib::Console::new().unwrap();

    loop {
        println!("Type any key.");
        let mut quit = false;

        console
            .run_loop(&mut quit, |quit, meta| {
                let cstr = unsafe {
                    // `from_ptr` takes *const c_char which is either i8 or u8 (platform
                    // depdendent).
                    CStr::from_ptr(mem::transmute::<_, &[c_char; 256]>(meta.input) as *const _)
                };

                let response = String::from_utf8_lossy(cstr.to_bytes()).to_string();

                println!("{:?}", response);

                if response == "\u{1b}" {
                    *quit = true;
                }

                console_lib::KeyResponse::Stop
            })
            .unwrap();

        if quit {
            println!("Quit");
            break;
        }
    }
}
