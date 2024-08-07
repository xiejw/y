use libc;
use std::mem::MaybeUninit;

pub struct Console {
    saved_term: libc::termios,
}

impl Console {
    pub fn new() -> Result<Console, String> {
        // Set up the term for cbreak mode.
        let mut term = MaybeUninit::<libc::termios>::uninit();
        unsafe {
            let rc = libc::tcgetattr(0, term.as_mut_ptr());
            if rc == -1 {
                return Err("failed to call tcgetattr".to_string());
            }
        }
        let mut term = unsafe { term.assume_init() };

        // At this point, console will be correctly dropped and reseted.
        let console = Console {
            saved_term: term.clone(),
        };

        term.c_lflag &= !(libc::ICANON | libc::ECHO);
        term.c_lflag |= libc::ISIG;
        term.c_iflag &= !libc::ICRNL;
        term.c_cc[libc::VMIN] = 1;
        term.c_cc[libc::VTIME] = 0;
        unsafe {
            let rc = libc::tcsetattr(0, libc::TCSAFLUSH, &term as *const _);
            if rc == -1 {
                return Err("failed to call tcsetattr".to_string());
            }
        }

        Ok(console)
    }
}

impl Drop for Console {
    fn drop(&mut self) {
        unsafe {
            // Reset, and just eat the rc
            let _ = libc::tcsetattr(0, libc::TCSAFLUSH, &self.saved_term as *const _);
        }
    }
}

pub enum KeyKind {
    Na,
    Enter,
}

pub struct KeyMetadata<'a> {
    pub kind: KeyKind,
    pub input: &'a [u8; 256],
}

#[derive(Debug)]
pub enum KeyErrorCode {
    Io,           // POSIX read io
    LengthExceed, // Input buf is overflow.
}

pub enum KeyResponse {
    Continue,
    Stop,
}

type KeyCallbackFn<T> = fn(&mut T, &KeyMetadata) -> KeyResponse;

impl Console {
    pub fn run_loop<T>(&mut self, t: &mut T, f: KeyCallbackFn<T>) -> Result<(), KeyErrorCode> {
        let mut buf: [u8; 256] = [0; 256];

        loop {
            unsafe {
                let c = libc::read(0, buf.as_mut_ptr() as *mut libc::c_void, 256);
                if c == -1 {
                    return Err(KeyErrorCode::Io);
                }
                if c == 256 {
                    return Err(KeyErrorCode::LengthExceed);
                }
                buf[c as usize] = 0;
            }

            // TODO assign kind
            let metadata = KeyMetadata {
                kind: KeyKind::Na,
                input: &buf,
            };

            let rc = f(t, &metadata);
            match rc {
                KeyResponse::Stop => return Ok(()),
                KeyResponse::Continue => return Ok(()),
            }
        }
    }
}
