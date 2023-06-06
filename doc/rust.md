## Rust

## Rustup and Install

- https://www.rust-lang.org/tools/install
- https://rust-lang.github.io/rustup/basics.html

```
rustup update    # upgrade rust
rustup doc       # open doc
rustup component remove rust-docs
```

## Cargo

```
cargo new <name> --bin --vcs none
cargo update            # updates all dependencies
cargo update -p regex   # updates just “regex”
cargo doc --open
```

## LLVM-IR

```
rustc --emit=llvm-ir --crate-type=lib -O main.rs
cargo rustc -- --emit=llvm-ir
```

## Mdbook

```
cargon install mdbook
mdbook serve --open
mdbook init <name_folder)
mdbook build
```

## Books
- https://doc.rust-lang.org/cargo/index.html
- https://rust-lang.github.io/rustup/
