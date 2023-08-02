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

fn main() {
    exec("sh", &["-c", "find -L . -type f -not -path '*/.*' | sort | sed -e \"s/^.\\///\" | xargs shasum -a 256"]);
}
