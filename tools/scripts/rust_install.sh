#!/bin/bash
#
# Install Rust via rustup (if missing), then slim it down:
#   - set rustup profile to minimal (future toolchains skip extras)
#   - add rustfmt component
#   - remove rust-docs and clippy components
#
# Usage: <script>

set -e

if ! command -v rustup >/dev/null 2>&1; then
    if [ -x "${HOME}/.cargo/bin/rustup" ]; then
        # Installed but not on PATH in this shell.
        . "${HOME}/.cargo/env"
    else
        echo "rustup not found, installing..."
        curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y --profile minimal
        # Make rustup available in this shell.
        . "${HOME}/.cargo/env"
    fi
fi

rustup set profile minimal

# Minimal profile excludes rustfmt; add it (no-op if already installed).
rustup component add rustfmt

# Remove components only if installed (removing a missing one errors).
installed="$(rustup component list --installed)"
for comp in rust-docs clippy; do
    if echo "${installed}" | grep -q "^${comp}"; then
        printf '\t- %s installed, \033[31mremoving\033[0m\n' "${comp}"
        rustup component remove "${comp}"
    else
        printf '\t- %s not installed, \033[33mskipping\033[0m\n' "${comp}"
    fi
done

echo "Installed components:"
rustup component list --installed | sed $'s/^/\t- /'
