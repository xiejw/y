use std::fs::File;
use std::io::Read;

fn main() {
    let count = 1000;

    let mut f = File::open("/etc/hosts").unwrap();
    let mut buf = vec![0; count];
    f.read(&mut buf).unwrap();
    print!("{}", String::from_utf8(buf).unwrap());
}
