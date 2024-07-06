use libc;
use std::ffi::CStr;
use std::mem::MaybeUninit;

fn main() {
    // Set up the term for cbreak mode.
    let mut term = MaybeUninit::<libc::termios>::uninit();
    unsafe {
        let rc = libc::tcgetattr(0, term.as_mut_ptr());
        if rc == -1 {
            panic!("failed");
        }
    }
    let mut term = unsafe { term.assume_init() };
    let save_term = term.clone();

    term.c_lflag &= !(libc::ICANON | libc::ECHO);
    term.c_lflag |= libc::ISIG;
    term.c_iflag &= !libc::ICRNL;
    term.c_cc[libc::VMIN] = 1;
    term.c_cc[libc::VTIME] = 0;
    unsafe {
        let rc = libc::tcsetattr(0, libc::TCSAFLUSH, &term as *const _);
        if rc == -1 {
            panic!("failed");
        }
    }

    // Read logic

    let mut buf: [u8; 256] = [0; 256];

    println!("Type any key.");
    unsafe {
        let c = libc::read(0, buf.as_mut_ptr() as *mut libc::c_void, 256);
        if c == -1 {
            panic!("failed");
        }
        if c == 256 {
            panic!("failed");
        }
        buf[c as usize] = 0;
        println!("{:?}", CStr::from_ptr(&buf as *const _).to_str().unwrap());
    }

    // Reset
    unsafe {
        let rc = libc::tcsetattr(0, libc::TCSAFLUSH, &save_term as *const _);
        if rc == -1 {
            panic!("failed");
        }
    }
}
