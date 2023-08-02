use std::env;
use std::path::Path;
use std::process::Command;

/// A simple wrapper to execute cmd with args.
///
/// This guarantess the cmd will run until the end (blocking fasion).
fn exec(cmd: &str, args: &[&str]) {
    let mut child = Command::new(cmd)
        .args(args)
        .spawn()
        .expect("failed to execute cmd");
    child.wait().expect("command wasn't running");
}

fn checksum() {
    exec("sh", &["-c", "find -L . -type f -not -path '*/.*' | sort | sed -e \"s/^.\\///\" | xargs shasum -a 256"]);
}

fn main() {
    let mut args = env::args();
    match args.len() {
        1 => checksum(),
        2 => {
            let path_str = args.nth(1).unwrap();
            let dir = Path::new(&path_str);
            env::set_current_dir(&dir).expect("fail to change working dir");
            checksum();
        }
        _ => panic!("too many arguments!"),
    }
}
