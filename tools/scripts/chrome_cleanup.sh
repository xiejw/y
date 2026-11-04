#!/bin/bash
#
# Clean up Google Chrome on macOS:
#   1. Delete old Chrome versions left inside the app bundle after updates.
#   2. Delete Chrome profile data (profiles, settings, logins, AI models).
#   3. Delete Chrome cache.
#
# Asks for confirmation before each step. Chrome must not be running.
#
# Usage: <script>

set -e

APP="/Applications/Google Chrome.app"
VERSIONS_DIR="${APP}/Contents/Frameworks/Google Chrome Framework.framework/Versions"
PROFILE_DIR="${HOME}/Library/Application Support/Google/Chrome"
CACHE_DIR="${HOME}/Library/Caches/Google/Chrome"

if [ "$(uname -s)" != "Darwin" ]; then
    echo "This script works on macOS only."
    exit 1
fi

if pgrep -x "Google Chrome" > /dev/null; then
    echo "Google Chrome is running. Quit it first."
    exit 1
fi

# Ask a yes/no question. Returns 0 on yes.
confirm() {
    local answer
    read -r -p "$1 [y/N] " answer
    case "${answer}" in
        y|Y|yes|YES) return 0 ;;
        *) return 1 ;;
    esac
}

size_of() {
    du -sh "$1" 2> /dev/null | cut -f1
}

# --- 1. Old Chrome versions ---------------------------------------------------

echo "==> Old Chrome versions"
if [ -d "${VERSIONS_DIR}" ]; then
    current="$(readlink "${VERSIONS_DIR}/Current")"
    echo "Current version: ${current}"

    old_versions=()
    for v in "${VERSIONS_DIR}"/*; do
        name="$(basename "${v}")"
        if [ "${name}" != "Current" ] && [ "${name}" != "${current}" ]; then
            old_versions+=("${v}")
        fi
    done

    if [ ${#old_versions[@]} -eq 0 ]; then
        echo "No old versions found."
    else
        for v in "${old_versions[@]}"; do
            echo "  $(basename "${v}") ($(size_of "${v}"))"
        done
        if confirm "Delete these old versions?"; then
            for v in "${old_versions[@]}"; do
                rm -rf "${v}"
            done
            echo "Deleted."
        else
            echo "Skipped."
        fi
    fi
else
    echo "Chrome is not installed at ${APP}."
fi
echo

# --- 2. Profile data ----------------------------------------------------------

echo "==> Chrome profile data"
if [ -d "${PROFILE_DIR}" ]; then
    echo "  ${PROFILE_DIR} ($(size_of "${PROFILE_DIR}"))"
    echo "  WARNING: removes all profiles, settings, and local logins."
    echo "           Synced data comes back after signing in again."
    if confirm "Delete Chrome profile data?"; then
        rm -rf "${PROFILE_DIR}"
        echo "Deleted."
    else
        echo "Skipped."
    fi
else
    echo "No profile data found."
fi
echo

# --- 3. Cache -----------------------------------------------------------------

echo "==> Chrome cache"
if [ -d "${CACHE_DIR}" ]; then
    echo "  ${CACHE_DIR} ($(size_of "${CACHE_DIR}"))"
    if confirm "Delete Chrome cache?"; then
        rm -rf "${CACHE_DIR}"
        echo "Deleted."
    else
        echo "Skipped."
    fi
else
    echo "No cache found."
fi
