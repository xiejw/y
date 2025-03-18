use std::fs::File;
use std::io::Read;

use anyhow::Result;

fn main() -> Result<()> {
    let count = 1000;

    let mut f = File::open("/etc/hosts")?;
    let mut buf = vec![0; count];
    f.read(&mut buf)?;
    print!("{}", String::from_utf8(buf)?);
    Ok(())
}
